#ifndef SERVER_HPP
#define SERVER_HPP

#include "ft_irc_utils.hpp"
#include <vector>
#include <map>
#include <poll.h>
#include <string>
#include <netinet/in.h>

class Client; // forward declaration

class Server {
public:
    Server(int port, const std::string &password);
    ~Server();

    // Ana döngüyü başlatır
    void launch();

    // Yeni bağlantıları kabul eder, mevcut istemcilerin verilerini işler
    void processEvents();
    void acceptClient();
    void receiveData(Client &client);
    void processClientMessage(Client &client);

    // İstemci henüz kayıtlı değilse kimlik doğrulama komutlarını işler (PASS, NICK, USER)
    void handleAuthentication(Client &client, const std::vector<std::string> &tokens);
    // Kayıtlı istemciden gelen komutları (şimdilik sadece QUIT) işler
    void handleRegisteredCommand(Client &client, const std::vector<std::string> &tokens);
    // İstemcinin bağlantısını sonlandırır
    void quitClient(Client &client);

    const std::string &getPassword() const;

private:
    int _port;
    std::string _password;
    int _serverSocket;
    struct sockaddr_in _serverAddr;
    std::vector<pollfd> _pollFds;
    std::map<int, Client> _clients; // socket fd'sine göre istemciler
    bool _running;
};

#endif
