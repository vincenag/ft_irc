#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <csignal>

class Server{
    public:
        Server();
        ~Server();
        Server(Server const &src);
        Server &operator=(Server const &src);

        //static void signalHandler(int signal);
    private:
        char *password;
        int port;
        static bool Signal;

};

#endif