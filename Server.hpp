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

    // Kimlik doğrulama için PASS, NICK, USER komutlarını işler
    void handleAuthentication(Client &client, const std::vector<std::string> &tokens);
    // Kayıtlı istemcilerde sadece QUIT komutunu işler
    void handleRegisteredCommand(Client &client, const std::vector<std::string> &tokens);
    // İstemcinin bağlantısını sonlandırır
    void quitClient(Client &client);

    // RFC 1459 uyumlu numeric reply göndermek için yardımcı metot
    void sendNumericReply(Client &client, const std::string &code, const std::string &msg);

    const std::string &getPassword() const;

private:
    int _port;
    std::string _password;
    int _serverSocket;
    struct sockaddr_in _serverAddr;
    std::vector<pollfd> _pollFds;
    std::map<int, Client> _clients; // socket fd'sine göre istemciler
    bool _running;
    std::string _serverName;
};

#endif
