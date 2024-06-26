#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include <string>
#include <sstream>
#include <map>
#include "Client.hpp"
#include "Server.hpp"

class CommandHandler 
{
    public:
        CommandHandler(Server &server);
        void handleCommand(Client &client, const std::string &commandLine);

    private:
        Server &server;

        // Metodos para procesar cada comando
        void processNick(Client &client, const std::vector<std::string> &args);
        void processUser(Client &client, const std::vector<std::string> &args);
        void processJoin(Client &client, const std::vector<std::string> &args);
        void processPrivmsg(Client &client, const std::vector<std::string> &args);
        void processKick(Client &client, const std::vector<std::string> &args);
        void processInvite(Client &client, const std::vector<std::string> &args);
        void processTopic(Client &client, const std::vector<std::string> &args);
        void processMode(Client &client, const std::vector<std::string> &args);
        
        // Funciones auxiliares
        std::vector<std::string> splitCommand(const std::string &commandLine);
};

#endif
