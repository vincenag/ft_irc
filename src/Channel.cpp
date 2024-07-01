#include "Channel.hpp"

Channel::Channel()
{
    this->name = "";
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

void Channel::AddUser(std::string user)
{
    this->users.push_back(user);
    std::cout << "User " << user << " added to chanel " << this->name << std::endl;
}

void Channel::RemoveUser(std::string user)
{
    this->users.erase(std::remove(this->users.begin(), this->users.end(), user), this->users.end());
}

std::vector<std::string> Channel::GetUsers()
{
    return this->users;
}