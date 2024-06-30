#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include "Library.hpp"
class Client;

class CommandHandler 
{
    public:
        void handleCommand(Client &client, const std::string &commandLine);

    private:
        // Comandos
        void processNick(Client& client, const std::vector<std::string>& args);
        
        // Funciones auxiliares
        std::vector<std::string> splitCommand(const std::string &commandLine);
};

#endif
