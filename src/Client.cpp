#include "Client.hpp"

Client::Client(): clientSocket(-1), clientIpAddr(""), clientNick(""), Authenticated(false),
                   nickSet(false), userSet(false), username(""), realname(""),
                   hostname(""), servername(""), buffer(""), disconnected(false){}

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
        this->nickSet = src.nickSet;
        this->userSet = src.userSet;
        this->username = src.username;
        this->realname = src.realname;
        this->hostname = src.hostname;
        this->servername = src.servername;
        this->buffer = src.buffer;
        this->disconnected = src.disconnected;
        this->commandQueue = src.commandQueue;
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

void Client::SetAuthenticated(bool Authenticated)
{
    this->Authenticated = Authenticated;
}
// Definir estado de autenticado de USER y NICK

void Client::SetNickSet(bool value)
{
    this->nickSet = value;
}

void Client::SetUserSet(bool value)
{
    this->userSet = value;
}
bool Client::getUser()
{
    return this->userSet;
}

bool Client::getNick()
{
    return this->nickSet;
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

std::string Client::GetUsername() const
{
    return this->username;
}

std::string Client::GetRealname() const
{
    return this->realname;
}

std::string Client::GetHostname() const
{
    return this->hostname;
}

std::string Client::GetServername() const
{
    return this->servername;
}


// ctrl + D

std::string &Client::getBuffer()
{
    return this->buffer;
}

// ctrl + Z

bool Client::IsDisconnected() const
{
    return this->disconnected;
}

void Client::SetDisconnected(bool state)
{
    this->disconnected = state;
}

std::queue<std::string> Client::getCommandQueue()
{
    return this->commandQueue;
}
