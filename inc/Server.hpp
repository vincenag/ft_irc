#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <csignal>
#include <exception>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <vector>
#include <fcntl.h>
#include <arpa/inet.h>
#include <algorithm>
#include <cstring>

#include "Client.hpp"

class Server{
    public:
        Server();
        ~Server();
        Server(Server const &src);
        Server &operator=(Server const &src);

        static void signalHandler(int signal);
        void serverInit(int port, std::string password);

        // Funciones auxiliares
        bool isNicknameTaken(const std::string &nickname);
        void joinChannel(const std::string &channel, Client &client);
        void sendMessage(const std::string &target, const std::string &message, Client &client);
        void kickUser(const std::string &channel, const std::string &user, Client &client);
        void inviteUser(const std::string &channel, const std::string &user, Client &client);
        void setTopic(const std::string &channel, const std::string &topic, Client &client);
        void setChannelMode(const std::string &channel, const std::string &mode, Client &client);

    private:
        std::string password;
        int port;
        static bool Signal;
        int serverSocket; // File descriptor for the server socket
        struct sockaddr_in address; // Server address
        std::vector<struct pollfd> fds; // Vector of file descriptors for the clients
        struct pollfd new_poll; // File descriptor for the new client
        std::vector<Client> clients; // Vector of clients
    
        void socketInit();
        void acceptClient();
        void getClientdata(int clientSocket);

};

#endif