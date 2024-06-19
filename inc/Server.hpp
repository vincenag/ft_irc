#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <csignal>
#include <exception>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>

class Server{
    public:
        Server();
        ~Server();
        Server(Server const &src);
        Server &operator=(Server const &src);

        static void signalHandler(int signal);
        void serverInit(int port, std::string password);
        void socketInit();

    private:
        std::string password;
        int port;
        static bool Signal;

        int serverSocket; // File descriptor for the server socket
        struct sockaddr_in address; // Server address

};

#endif