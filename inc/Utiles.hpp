#ifndef UTILES_HPP
#define UTILES_HPP

#include "Library.hpp"
#include "Client.hpp"


class Utiles
{
    private:
        
    public:
        static void sendWelcomeMessage(Client &client);
        static void sendNumericReply(Client &client, int code, const std::string &msg);
        static std::string toString(int value);
};

#endif