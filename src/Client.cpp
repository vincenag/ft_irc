#include "Client.hpp"

Client::Client(): clientSocket(-1), clientIpAddr(""){}

Client::~Client(){}

Client::Client(Client const &src){*this = src;}

Client &Client::operator=(Client const &src)
{
    if (this != &src)
    {
        this->clientSocket = src.clientSocket;
        this->clientIpAddr = src.clientIpAddr;
    }
    return *this;
}

void Client::SetClientSocket(int clientSocket)
{
    this->clientSocket = clientSocket;
}

void Client::SetClientIpAddr(std::string clientIpAddr)
{
    this->clientIpAddr = clientIpAddr;
}

int Client::GetClientSocket()
{
    return this->clientSocket;
}

std::string Client::GetClientIpAddr()
{
    return this->clientIpAddr;
}

std::string Client::GetClientNick()
{
    return this->clientNick;
}