#include "Client.hpp"

Client::Client(){}

Client::~Client(){}

Client::Client(Client const &src){*this = src;}

Client &Client::operator=(Client const &src)
{
    if (this != &src)
    {
        *this = src;
    }
    return *this;
}