#include "Server.hpp"
#include "Client.hpp"
#include "ft_irc_utils.hpp"
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <cerrno>

Server::Server(int port, const std::string &password)
    : _port(port), _password(password), _running(true), _serverName("irc.ft_irc.com")
{
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket < 0)
        throw std::runtime_error("Socket creation failed");

    int opt = 1;
    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(_serverSocket);
        throw std::runtime_error("setsockopt failed");
    }

    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_addr.s_addr = INADDR_ANY;
    _serverAddr.sin_port = htons(_port);

    if (bind(_serverSocket, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) < 0) {
        close(_serverSocket);
        throw std::runtime_error("Bind failed");
    }

    if (listen(_serverSocket, 10) < 0) {
        close(_serverSocket);
        throw std::runtime_error("Listen failed");
    }

    pollfd pfd;
    pfd.fd = _serverSocket;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _pollFds.push_back(pfd);

    std::cout << "Server listening on port " << _port << std::endl;
}

Server::~Server() {
    for (size_t i = 0; i < _pollFds.size(); ++i) {
        close(_pollFds[i].fd);
    }
    std::cout << "Server shutting down." << std::endl;
}

void Server::launch() {
    while (_running) {
        int ret = poll(&_pollFds[0], _pollFds.size(), -1);
        if (ret < 0)
            throw std::runtime_error("poll error");

        // Yeni bağlantı varsa
        if (_pollFds[0].revents & POLLIN)
            acceptClient();

        // Mevcut istemcilerden gelen verileri işle
        processEvents();
    }
}

void Server::acceptClient() {
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int clientFd = accept(_serverSocket, (struct sockaddr *)&clientAddr, &addrLen);
    if (clientFd < 0) {
        std::cerr << "Accept failed: " << strerror(errno) << std::endl;
        return;
    }

    // Non-blocking mod ayarı (yalnızca O_NONBLOCK kullanılıyor)
    int flags = fcntl(clientFd, F_GETFL, 0);
    fcntl(clientFd, F_SETFL, flags | O_NONBLOCK);

    pollfd pfd;
    pfd.fd = clientFd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _pollFds.push_back(pfd);

    _clients[clientFd] = Client(clientFd);
    _clients[clientFd].sendMessage("Welcome to ft_irc. Please authenticate using PASS, NICK and USER.");
    std::cout << "New client connected: FD " << clientFd << std::endl;
}

void Server::processEvents() {
    // İlk pollfd sunucu soketi olduğundan, index 1'den başlıyoruz.
    for (size_t i = 1; i < _pollFds.size(); ++i) {
        if (_pollFds[i].revents & POLLIN) {
            int fd = _pollFds[i].fd;
            if (_clients.find(fd) == _clients.end())
                continue;
            receiveData(_clients[fd]);
            processClientMessage(_clients[fd]);
        }
    }
}

void Server::receiveData(Client &client) {
    char buf[1024];
    std::memset(buf, 0, sizeof(buf));
    int bytes = recv(client.getFd(), buf, sizeof(buf) - 1, 0);
    if (bytes <= 0) {
        // Bağlantı koptuysa, istemciyi sonlandır
        quitClient(client);
        return;
    }
    client.appendBuffer(std::string(buf, bytes));
}

void Server::processClientMessage(Client &client) {
    std::string line;
    while (client.getLine(line)) {
        std::vector<std::string> tokens = split(line, ' ');
        if (tokens.empty())
            continue;
        if (!client.isRegistered()) {
            // Sadece PASS, NICK ve USER komutlarına izin veriyoruz
            if (tokens[0] == "PASS" || tokens[0] == "NICK" || tokens[0] == "USER")
                handleAuthentication(client, tokens);
            else
                client.sendMessage("ERROR: You must authenticate using PASS, NICK and USER.");
        } else {
            // Kayıtlı istemciler için yalnızca QUIT komutu kabul ediliyor.
            if (tokens[0] == "QUIT") {
                handleRegisteredCommand(client, tokens);
                // QUIT işlendiğinde, ek gelen verileri işleme döngüsünden çıkıyoruz.
                break;
            } else {
                client.sendMessage("ERROR: Unknown command. Only QUIT command is allowed.");
            }
        }
    }
}

void Server::handleAuthentication(Client &client, const std::vector<std::string> &tokens) {
    if (tokens[0] == "PASS") {
        if (tokens.size() < 2) {
            client.sendMessage("461 PASS :Not enough parameters");
            return;
        }
        if (tokens[1] == _password)
            client.setPass(true);
        else {
            client.sendMessage("464 :Password incorrect");
            return;
        }
    } else if (tokens[0] == "NICK") {
        if (tokens.size() < 2) {
            client.sendMessage("461 NICK :Not enough parameters");
            return;
        }
        client.setNickname(tokens[1]);
    } else if (tokens[0] == "USER") {
        if (tokens.size() < 2) {
            client.sendMessage("461 USER :Not enough parameters");
            return;
        }
        client.setUsername(tokens[1]);
    }

    // Eğer PASS, NICK ve USER bilgileri sağlanmışsa istemciyi kayıtlı hale getir
    if (client.hasPassed() && !client.getNickname().empty() && !client.getUsername().empty()) {
        client.setRegistered(true);
        // RFC 1459'a uygun welcome mesajı: Numeric reply 001
        sendNumericReply(client, "001", "Welcome to the IRC network, " + client.getNickname());
    }
}

void Server::handleRegisteredCommand(Client &client, const std::vector<std::string> &tokens) {
    // Sadece QUIT komutu kabul ediliyor
    if (tokens[0] == "QUIT")
        quitClient(client);
    else
        client.sendMessage("ERROR: Unknown command. Only QUIT command is allowed.");
}

void Server::quitClient(Client &client) {
    int fd = client.getFd();
    client.sendMessage("Goodbye!");
    // Soket üzerindeki tüm okuma ve yazma işlemlerini durduruyoruz
    shutdown(fd, SHUT_RDWR);
    close(fd);

    // pollfd listesinden istemci soketini kaldırıyoruz
    for (size_t i = 0; i < _pollFds.size(); ++i) {
        if (_pollFds[i].fd == fd) {
            _pollFds.erase(_pollFds.begin() + i);
            break;
        }
    }
    _clients.erase(fd);
    std::cout << "Client FD " << fd << " disconnected." << std::endl;
}

void Server::sendNumericReply(Client &client, const std::string &code, const std::string &msg) {
    // Örnek: ":irc.ft_irc.com 001 <nickname> :Welcome to the IRC network, <nickname>"
    std::string reply = ":" + _serverName + " " + code + " " + client.getNickname() + " :" + msg;
    client.sendMessage(reply);
}

const std::string &Server::getPassword() const {
    return _password;
}
