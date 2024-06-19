#include "Server.hpp"

Server::Server(){}

Server::~Server(){}

Server::Server(Server const &src){*this = src;}

Server &Server::operator=(Server const &src)
{
    if (this != &src)
    {
        *this = src;
    }
    return *this;
}

/* void Server::signalHandler(int signal)
{
    (void)signal;
    std::cout << "Signal received" << std::endl;
    Signal = true;
} */