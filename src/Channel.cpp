#include "Channel.hpp"

Channel::Channel()
{
    this->name = "";
    this->users = std::vector<int>();
    this->operators = std::set<int>();
    this->InviteOnly = false;
    this->Topicblock = false;
    this->topic = "";
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

bool Channel::UserExists(int clientSocket)
{
    return std::find(this->users.begin(), this->users.end(), clientSocket) != this->users.end();
}


bool Channel::isOperator(int clientSocket) const
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

bool Channel::IsInvited(int clientSocket)
{
    return this->invitedUsers.find(clientSocket) != this->invitedUsers.end();
}

bool Channel::isTopicblock() const
{
    return this->Topicblock;
}

void Channel::setTopicblock(bool value)
{
    this->Topicblock = value;
}

void Channel::setTopic(std::string topic)
{
    this->topic = topic;
}

std::string Channel::getTopic()
{
    return this->topic;
}