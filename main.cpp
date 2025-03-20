#include "ft_irc_utils.hpp"
#include "Server.hpp"
#include <cstdlib>
#include <iostream>
#include <csignal>

int main(int argc, char** argv) {
    // SIGPIPE sinyalini yoksay, böylece bağlantı kapandığında hatalar oluşmaz.
    signal(SIGPIPE, SIG_IGN);

    if (argc != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }
    int port = std::atoi(argv[1]);
    std::string password(argv[2]);
    try {
        Server server(port, password);
        server.launch();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
