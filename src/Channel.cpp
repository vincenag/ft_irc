#include "Channel.hpp"

Channel::Channel()
{
    this->name = "";
    this->users = std::vector<int>();
    this->operators = std::set<int>();
}

Channel::~Channel(){}

Channel::Channel(Channel const &src)
{
    *this = src;
}

Channel &Channel::operator=(Channel const &src)
{
    if (this != &src)
    {
        this->name = src.name;
        this->users = src.users;
        this->operators = src.operators;
    }
    return *this;
}

Channel::Channel(std::string name)
{
    this->name = name;
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
