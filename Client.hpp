#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>

class Client {
public:
    Client();
    Client(int fd);

    int getFd() const;
    const std::string &getBuffer() const;
    void appendBuffer(const std::string &data);
    // Buffer'da \r\n ile biten tam bir satır varsa bunu alır
    bool getLine(std::string &line);
    void sendMessage(const std::string &message);

    // Kayıt durumu ve kimlik bilgileri
    bool isRegistered() const;
    void setRegistered(bool status);
    const std::string &getNickname() const;
    void setNickname(const std::string &nickname);
    const std::string &getUsername() const;
    void setUsername(const std::string &username);
    bool hasPassed() const;
    void setPass(bool passed);

private:
    int _fd;
    std::string _buffer;
    bool _registered;
    bool _pass;
    std::string _nickname;
    std::string _username;
};

#endif
