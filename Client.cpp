#include "Client.hpp"
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <sys/socket.h>
#include <cerrno>

Client::Client() : _fd(-1), _registered(false), _pass(false) {}

Client::Client(int fd) : _fd(fd), _registered(false), _pass(false) {}

int Client::getFd() const {
    return _fd;
}

const std::string &Client::getBuffer() const {
    return _buffer;
}

void Client::appendBuffer(const std::string &data) {
    _buffer.append(data);
}

bool Client::getLine(std::string &line) {
    size_t pos = _buffer.find("\r\n");
    if (pos != std::string::npos) {
        line = _buffer.substr(0, pos);
        _buffer.erase(0, pos + 2);
        return true;
    }
    return false;
}

void Client::sendMessage(const std::string &message) {
    std::string msg = message + "\r\n";
    ssize_t sent = send(_fd, msg.c_str(), msg.size(), 0);
    if (sent < 0) {
        if (errno == EPIPE)
            return; // Broken pipe hatası durumunda sessizce geri dön "nc: write failed (0/2): Broken pipe" hatası için
        // Diğer hatalar için loglama yapılabilir
    }
}
bool Client::isRegistered() const {
    return _registered;
}

void Client::setRegistered(bool status) {
    _registered = status;
}

const std::string &Client::getNickname() const {
    return _nickname;
}

void Client::setNickname(const std::string &nickname) {
    _nickname = nickname;
}

const std::string &Client::getUsername() const {
    return _username;
}

void Client::setUsername(const std::string &username) {
    _username = username;
}

bool Client::hasPassed() const {
    return _pass;
}

void Client::setPass(bool passed) {
    _pass = passed;
}
