#include "Client.hpp"

// ############################################################
// #                                                          #
// #                     CONSTRUCTORS                         #
// #                                                          #
// ############################################################

Client::Client():   clientSocket(0), clientIpAddr(""), clientNick(""), Authenticated(false), 
                    nickSet(false), userSet(false), username(""), realname(""), 
                    hostname(""), servername(""), buffer(""), disconnected(false), 
                    registered(false), currentChannel("") {}

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
        this->registered = src.registered;
        this->currentChannel = src.currentChannel;
    }
    return *this;
}

// ############################################################
// #                                                          #
// #                         GETTERS                          #
// #                                                          #
// ############################################################

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

std::string Client::GetChannel() const
{
    return this->currentChannel;
}

void Client::SetChannel(const std::string &channel)
{
    this->currentChannel = channel;
}

// ############################################################
// #                                                          #
// #                         NICK                             #
// #                                                          #
// ############################################################

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

bool Client::isValidNick(const std::string &nick)
{
    if (nick.length() > 9)
        return false;
    for (size_t i = 0; i < nick.length(); i++)
    {
        if (!std::isalnum(nick[i]) && nick[i] != '_')
            return false;
    }
    return true;
}

// ############################################################
// #                                                          #
// #                         USER                             #
// #                                                          #
// ############################################################

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

bool Client::IsRegistered() const
{
    return this->registered;
}

void Client::SetRegistered(bool value)
{
    this->registered = value;
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


// ############################################################
// #                                                          #
// #                         BUFFER CTRL + D                  #
// #                                                          #
// ############################################################

std::string &Client::getBuffer()
{
    return this->buffer;
}

// ############################################################
// #                                                          #
// #                         DISCONNECTED CTRL + Z            #
// #                                                          #
// ############################################################

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

void Client::clearBuffer() {
    this->buffer.clear();
}