#ifndef SERVER_HPP
#define SERVER_HPP

#include "Library.hpp"
class Client;
//class CommandHandler;

class Server{
    public:
        Server();
        ~Server();
        Server(Server const &src);
        Server &operator=(Server const &src);

        static void signalHandler(int signal);
        void serverInit(int port, std::string password);

    private:
        std::string password;
        int port;
        static bool Signal;
        int serverSocket; // File descriptor for the server socket
        struct sockaddr_in address; // Server address
        std::vector<struct pollfd> fds; // Vector of file descriptors for the clients
        struct pollfd new_poll; // File descriptor for the new client
        std::vector<Client> clients; // Vector of clients
        std::vector<Channel> channels; // Vector of chanels
        std::map<int, std::string> clientData; // Map of fd y Nickname    
    
        void socketInit();
        void acceptClient();
        void getClientdata(int clientSocket);

};

#endif