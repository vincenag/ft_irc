#include "Channel.hpp"

Channel::Channel()
{
    this->name = "";
    this->users = std::vector<int>();
    this->operators = std::set<int>();
    this->InviteOnly = false;
    this->Topicblock = false;
    this->topic = "";
    this->password = "";
    this->limitUsers = 0;
    this->limitUsersEnabled = false;
    this->modeK = false;
}

Channel::~Channel(){}

Channel::Channel(Channel const &src)
{
    this->name = src.name;
    this->users = src.users;
    this->operators = src.operators;
    this->InviteOnly = src.InviteOnly;
    this->invitedUsers = src.invitedUsers;
    this->topic = src.topic;
    this->Topicblock = src.Topicblock;
    this->password = src.password;
    this->limitUsers = src.limitUsers;
    this->limitUsersEnabled = src.limitUsersEnabled;
    this->modeK = src.modeK;
}

Channel &Channel::operator=(Channel const &src)
{
    if (this != &src)
    {
        this->name = src.name;
        this->users = src.users;
        this->operators = src.operators;
        this->InviteOnly = src.InviteOnly;
        this->invitedUsers = src.invitedUsers;
        this->topic = src.topic;
        this->Topicblock = src.Topicblock;
        this->password = src.password;
        this->limitUsers = src.limitUsers;
        this->limitUsersEnabled = src.limitUsersEnabled;
        this->modeK = src.modeK;
    }
    return *this;
}

Channel::Channel(std::string name)
{
    this->name = name;
    this->users = std::vector<int>();
    this->operators = std::set<int>();
    this->InviteOnly = false;
    this->Topicblock = false;
    this->topic = "";
    this->password = "";
    this->limitUsers = 0;
    this->limitUsersEnabled = false;
    this->modeK = false;
}

void Channel::SetName(std::string name)
{
    this->name = name;
}

std::string Channel::GetName()
{
    return this->name;
}

void Channel::AddUser(int clientSocket)
{
    this->users.push_back(clientSocket);
}

void Channel::RemoveUser(int clientSocket)
{
    this->users.erase(std::remove(this->users.begin(), this->users.end(), clientSocket), this->users.end());
}

std::vector<int> Channel::GetUsers()
{
    return this->users;
}
// --------- USUARIOS -------------

bool Channel::UserExists(int clientSocket)
{
    return std::find(this->users.begin(), this->users.end(), clientSocket) != this->users.end();
}

bool Channel::IsUserInChannel(int clientSocket) const
{
    return this->users.size() > 0 && std::find(this->users.begin(), this->users.end(), clientSocket) != this->users.end();
}

// --------- OPERADORES -------------

bool Channel::IsOperator(int clientSocket) const
{
    return this->operators.find(clientSocket) != this->operators.end();
}

void Channel::addOperator(int clientSocket)
{
    this->operators.insert(clientSocket);
}

void Channel::removeOperator(int clientSocket)
{
    this->operators.erase(clientSocket);
}

void Channel::SetOperator(int clientSocket, bool isOperator)
{
    if (isOperator)
    {
        this->addOperator(clientSocket);
    }
    else
    {
        this->removeOperator(clientSocket);
    }
}

// --------- INVITADOS -------------

bool Channel::isInviteOnly() const
{
    return this->InviteOnly;
}

void Channel::setInviteOnly(bool value)
{
    this->InviteOnly = value;
}

void Channel::inviteUser(int clientSocket)
{
    this->invitedUsers.insert(clientSocket);
}

bool Channel::IsInvited(int clientSocket) const
{
    return this->invitedUsers.find(clientSocket) != this->invitedUsers.end();
}

// --------- KICK -------------

void Channel::KickUser(int userSocket, const std::string &reason)
{
    // Enviar mensaje de KICK a todos los usuarios del canal
    std::vector<int>::iterator it;
    for (it = this->users.begin(); it != this->users.end(); ++it)
    {
        int clientSocket = *it;
        if (clientSocket != userSocket)
        {
            std::ostringstream ss;
            ss << ":" << userSocket << " KICK " << this->name << " " << userSocket << " :" << reason << "\r\n";
            std::string msg = ss.str();
            send(clientSocket, msg.c_str(), msg.size(), 0);
        }
    }
    // Eliminar al usuario del canal
    this->RemoveUser(userSocket);
}

void Channel::BroadcastMessage(const std::string &message, Server &server)
{
    std::vector<int>::iterator it;
    for (it = this->users.begin(); it != this->users.end(); ++it)
    {
        int clientSocket = *it;
        Client *client = server.GetThisClient(clientSocket);
        if (client != nullptr)
        {
            std::ostringstream ss;
            if (isTopicChanged(message)) {
                ss << message;
            } else {
                ss << ":" << client->GetClientSocket() << " PRIVMSG " << this->name << " :" << message << "\r\n";
            }
            std::string msg = ss.str();
            send(clientSocket, msg.c_str(), msg.size(), 0);
        }
    }
}

bool Channel::isTopicChanged(const std::string &topic) const
{
    return this->topic != topic;
}

// --------- TOPIC -------------

bool Channel::isTopicblock() const
{
    return this->Topicblock;
}

void Channel::setTopicblock(bool value)
{
    this->Topicblock = value;
}

const std::string &Channel::getTopic() const
{
    return this->topic;
}

void Channel::setTopic(const std::string &newTopic)
{
    this->topic = newTopic;
}

bool Channel::HasTopic() const
{
    return this->topic != "";
}

// --------- PASSWORD -------------

void Channel::setPassword(std::string password)
{
    this->password = password;
}

std::string Channel::getPassword() const
{
    return this->password;
}

void Channel::setLimitUsers(int limit)
{
    this->limitUsers = limit;
}

unsigned int Channel::getLimitUsers() const
{
    return this->limitUsers;
}

// --------- MODOS DEL CANAL -------------

bool Channel::isLimitUsersEnabled() const
{
    return this->limitUsersEnabled;
}

void Channel::setLimitUsersEnabled(bool value)
{
    this->limitUsersEnabled = value;
}

bool Channel::isModeKEnabled() const
{
    return this->modeK;
}

void Channel::setModeKEnabled(bool value)
{
    this->modeK = value;
}