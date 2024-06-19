#include "Server.hpp"

Server::Server(){}

Server::~Server(){}

Server::Server(Server const &src){*this = src;}

Server &Server::operator=(Server const &src) // modificar esto luego
{
    if (this != &src)
    {
        *this = src;
    }
    return *this;
}

bool Server::Signal = false;

void Server::signalHandler(int signal)
{
    (void)signal;
    std::cout << "Signal received" << std::endl;
    Signal = true;
}

void Server::serverInit(int port, std::string password)
{
    this->port = port;
    this->password = password;

    std::cout << "Server initialized" << std::endl;

    this->socketInit();

    //.............................................

}

void Server::socketInit() 
{

    this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->serverSocket == -1)
    {
        std::cerr << "Error creating socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    this->address.sin_family = AF_INET;         // Address family = Internet
    this->address.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY allows the server to accept a client connection on any interface, so you can use any IP address on the system
    this->address.sin_port = htons(this->port); // htons() converts the port number to network byte order

    if (bind(this->serverSocket, (struct sockaddr *)&this->address, sizeof(this->address)) < 0) // Bind the socket to the address and port number specified in addr
    {
        std::cerr << "Error binding socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(this->serverSocket, SOMAXCONN) < 0)  // Listen for incoming connections
    {
        std::cerr << "Error listening" << std::endl;
        exit(EXIT_FAILURE);
    }

    //.............................................
}