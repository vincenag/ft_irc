#include "Client.hpp"

Client::Client(): clientSocket(-1), clientIpAddr(""), nickSet(false), userSet(false){}

Client::~Client(){}

Client::Client(Client const &src){*this = src;}

Client &Client::operator=(Client const &src)
{
    if (this != &src)
    {
        this->clientSocket = src.clientSocket;
        this->clientIpAddr = src.clientIpAddr;
        this->clientNick = src.clientNick;
        this->Authenticated = src.Authenticated;

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

int Client::GetClientSocket() const
{
    return this->clientSocket;
}

std::string Client::GetClientIpAddr() const
{
    return this->clientIpAddr;
}

std::string Client::GetClientNick() const
{
    return this->clientNick;
}

void Client::SetClientNick(std::string clientNick)
{
    this->clientNick = clientNick;
}

bool Client::GetAuthenticated()
{
    return this->Authenticated;
}

// Definir estado de autenticado de USER y NICK

void Client::SetAuthenticated(bool Authenticated)
{
    this->Authenticated = Authenticated;
}

void Client::SetNickSet(bool value)
{
    this->nickSet = value;
}

void Client::SetUserSet(bool value)
{
    this->userSet = value;
}

bool Client::IsFullyAuthenticated() const
{
    if (this->nickSet && this->userSet)
        return true;
    return false;
}

// Metodos para USER

void Client::SetUsername(const std::string &username)
{
    this->username = username;
}

void Client::SetRealname(const std::string &realname)
{
    this->realname = realname;
}

void Client::SetHostname(const std::string &hostname)
{
    this->hostname = hostname;
}

void Client::SetServername(const std::string &servername)
{
    this->servername = servername;
}
