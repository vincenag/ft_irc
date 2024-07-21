#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include "Library.hpp"
class Client;
class Server;
class Utiles;
class IRCBot;

class CommandHandler 
{
    public:
        void handleCommand(const std::string &commandLine, Server &server, Client &client);
        void processDCCSend(Client &client, const std::vector<std::string> &args);
        void sendMessageToBot(Client &client, Server &server, const std::vector<std::string> &args);
    private:
        // Comandos
        void processPass(Client &client, Server &server, const std::vector<std::string> &args);
        void processNick(Client &client, Server &server, const std::vector<std::string> &args);
        void processUser(Client &client, Server &server, const std::vector<std::string> &args);
        void processJoin(Client &client, Server &server, const std::vector<std::string> &args);
        void processPrivmsg(Client &client, Server &server, const std::vector<std::string> &args);
        void processKick(Client &client, Server &server, const std::vector<std::string> &args);
        void processInvite(Client &client, Server &server, const std::vector<std::string> &args);
        void processTopic(Client &client, Server &server, const std::vector<std::string> &args);
        void processMode(Client &client, Server &server, const std::vector<std::string> &args);
        void ListChannels(Client &client, Server &server);

        // Funciones auxiliares
        std::vector<std::string> splitCommand(const std::string &commandLine);
        // metodo para manejar los comandos de operador
        bool handleOperatorCommand(Client &client, Server &server, const std::string &command, 
                                    const std::vector<std::string> &args);
        
};

#endif