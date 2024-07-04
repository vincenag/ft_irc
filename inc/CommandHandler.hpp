#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include "Library.hpp"
class Client;
class Server;

class CommandHandler 
{
    public:
        void handleCommand(const std::string &commandLine, Server &server, Client &client);

    private:
        // Comandos
        void processPass(Client &client, Server &server, const std::vector<std::string> &args);
        void processNick(Client &client, Server &server, const std::vector<std::string> &args);
        void processJoin(Client &client, Server &server, const std::vector<std::string> &args);
        void processPrivmsg(Client &client, Server &server, const std::vector<std::string> &args);


        // Funciones auxiliares
        std::vector<std::string> splitCommand(const std::string &commandLine);
        void sendToChannel(Server &server, const std::string &channelName, const std::string &msg, Client &client);
        void sendToClient(Server &server, const std::string &clientNick, const std::string &msg, Client &client);
};

#endif