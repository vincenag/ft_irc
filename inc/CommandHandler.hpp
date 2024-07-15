#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include "Library.hpp"
class Client;
class Server;
class Utiles;

class CommandHandler 
{
    public:
        void handleCommand(const std::string &commandLine, Server &server, Client &client);
    
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


        // Funciones auxiliares
        std::vector<std::string> splitCommand(const std::string &commandLine);
        void sendToChannel(Server &server, const std::string &channelName, const std::string &msg, Client &client);
        void sendToClient(Server &server, const std::string &clientNick, const std::string &msg, Client &client);

        bool isNumber(const std::string &s);
        
        // metodo para manejar los comandos de operador
        bool handleOperatorCommand(Client &client, Server &server, const std::string &command, 
                                    const std::vector<std::string> &args);
        
};

#endif