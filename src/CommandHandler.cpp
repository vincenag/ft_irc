#include "Library.hpp"

void CommandHandler::handleCommand(const std::string &commandLine, Server &server, Client &client)
{
    // hacemos un split del comando entero y lo guardamos en un vector
    std::vector<std::string> tokens = splitCommand(commandLine);


    // verificamos si esta vacio o solo tiene espacios
    if (tokens.empty())
        return;

    // extraemos el comando (primer elemento) y lo eliminamos
    // dejamos solo los argumentos del comando en tokens
    std::string command = tokens[0];
    tokens.erase(tokens.begin());

    // segun el comando entramos en la funcion
    // mandamos el cliente y el vector tokens con los argumentos
    if (command == "PASS" && client.GetAuthenticated() == false){
        processPass(client, server, tokens);
    } else if (command == "NICK" && client.GetAuthenticated() == true){
        processNick(client, server, tokens);
    } else if (command == "JOIN" && client.GetAuthenticated() == true && client.GetClientNick() != ""){
        processJoin(client, server, tokens);
    } else if (command == "PRIVMSG" && client.GetAuthenticated() == true && client.GetClientNick() != ""){
        processPrivmsg(client, server, tokens);
    } else if ((command == "KICK" || command == "INVITE" || command == "TOPIC" || command == "MODE") && client.GetAuthenticated() == true) {
        if (!tokens.empty()) {
            std::string channelName = tokens[0];
            handleOperatorCommand(client, server, channelName, command, tokens);
        } else {
            std::string Msg = RED "ERROR: Command requires a channel\n" RESET;
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        }
    } else {
        std::string msg = RED "ERROR: Unknown command\n" RESET;
        send(client.GetClientSocket(), msg.c_str(), msg.size(), 0);
    }
}

std::vector<std::string> CommandHandler::splitCommand(const std::string &commandLine)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(commandLine);
    while (std::getline(tokenStream, token, ' ')) {
        token.erase(std::remove(token.begin(), token.end(), '\r'), token.end());
        token.erase(std::remove(token.begin(), token.end(), '\n'), token.end());
        tokens.push_back(token);
    }
    return tokens;
}

void CommandHandler::processPass(Client &client, Server &server, const std::vector<std::string> &args)
{
    std::string Msg = RED "ERROR: Invalid password, you have been disconected.\n" RESET;

    std::string password = args[0];
    if (server.GetPassword() == password) {
        client.SetAuthenticated(true);
        std::cout   << Server::getCurrentTime() 
                    << GREEN << "[+] Client <" << client.GetClientSocket() << "> authenticated" << RESET << std::endl;
        Msg = GREEN "You have been authenticated. Use NICK command to continue\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    } else {
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        std::cout   << Server::getCurrentTime() 
                    << RED << "[-] Client <" << client.GetClientSocket() << "> failed to authenticate" << RESET << std::endl;
        server.RemoveClient(client.GetClientSocket());
        std::cout   << Server::getCurrentTime() 
                    << BLUE << "Client <" << client.GetClientSocket() << "> removed" << RESET << std::endl;
    }
}

void CommandHandler::processNick(Client &client, Server &server, const std::vector<std::string> &args)
{
    std::string Msg;

    if (args.size() < 1) {
        Msg = RED "ERROR: NICK command requires a Nickname: " RESET "NICK <nickname>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    std::string nick = args[0];

    // Si el cliente ya tiene este nick, enviamos un mensaje de error
    if (client.GetClientNick() == nick) {
        Msg = RED "ERROR: Nickname already set\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // Comprobar si el nick ya está en uso en el server
    for (size_t i = 0; i < server.GetClients().size(); i++) {
        if (server.GetClients()[i].GetClientNick() == nick && server.GetClients()[i].GetClientSocket() != client.GetClientSocket()){
            Msg = RED "ERROR: Nickname already in use\n" RESET;
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            // Obviar estos mensajes en el server, creo que no son necesarios, solo mostrar en el cliente
            /* std::cout   << Server::getCurrentTime() 
                        << RED << "[-] Client <" << client.GetClientSocket() << "> failed to set nickname" << RESET << std::endl; */
            return;
        }
    }

    // Si el nick es válido, lo establecemos
    client.SetClientNick(nick);
    std::cout   << Server::getCurrentTime() 
                << GREEN << "[+] Client <" << client.GetClientSocket() << "> set nickname to " 
                << MAGENTA << nick << RESET << std::endl;
    Msg = GREEN "Your Nickname has been set. Use JOIN command for create channel\n" RESET;
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
}

void CommandHandler::processJoin(Client &client, Server &server, const std::vector<std::string> &args)
{
    std::string Msg;
    if (args.size() < 1) {
        Msg = RED "ERROR: JOIN command requires a channel " RESET "JOIN <#channel>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    std::string channel = args[0];
    if (channel[0] != '#') {
        Msg = RED "ERROR: Channel name must start with '#': " RESET "JOIN <#channel>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // Usa el servidor para añadir el cliente al canal
    server.JoinChannel(client, channel);
}

void CommandHandler::processPrivmsg(Client &client, Server &server, const std::vector<std::string> &args)
{
    std::string Msg;
    if (args.size() < 2) {
        Msg = RED "ERROR: PRIVMSG command requires a channel and a message: " RESET "PRIVMSG <nick or #channel> :<message>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    std::string channel = args[0];
    std::string message;

    // Construir el mensaje completo
    for (size_t i = 1; i < args.size(); ++i) {
        if (i > 1)
            message += " ";
        message += args[i];
    }

    // Verificar si el mensaje tiene el formato correcto
    size_t pos = message.find(":");
    if (pos == std::string::npos) {
        Msg = RED "ERROR: Invalid message format. Use PRIVMSG <nick or #channel> :<message>\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    std::string msgContent = message.substr(pos + 1); // Obtener el contenido del mensaje

    //Buscar el destinatario
    if (channel[0] == '#') {
        sendToChannel(server, channel, msgContent, client);
    } else {
        sendToClient(server, channel, msgContent, client);
    }
    
}

void CommandHandler::processKick(Client &client, Server &server, const std::vector<std::string> &args)
{
    std::string Msg;
    if (args.size() < 2) {
        Msg = RED "ERROR: KICK command requires a channel and a nickname: " RESET "KICK <#channel> <nickname>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    std::string channel = args[0];
    std::string nick = args[1];

    // Buscar el canal y el cliente
    for (size_t i = 0; i < server.GetChannels().size(); ++i) {
        if (server.GetChannels()[i].GetName() == channel) {
            if (server.GetChannels()[i].isOperator(client.GetClientSocket())) {
                for (size_t j = 0; j < server.GetChannels()[i].GetUsers().size(); ++j) {
                    if (server.GetChannels()[i].GetUsers()[j] == client.GetClientSocket()) {
                        std::string Msg = RED "ERROR: You can't kick yourself\n" RESET;
                        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
                        return;
                    }
                    if (server.GetClients()[j].GetClientNick() == nick) {
                        server.GetChannels()[i].RemoveUser(server.GetClients()[j].GetClientSocket());
                        std::string Msg = GREEN "You have been kicked from the channel\n" RESET;
                        send(server.GetClients()[j].GetClientSocket(), Msg.c_str(), Msg.size(), 0);
                        return;
                    }
                }
                std::string Msg = RED "ERROR: User does not exist in the channel\n" RESET;
                send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
                return;
            } else {
                std::string Msg = RED "ERROR: You are not an operator in this channel\n" RESET;
                send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
                return;
            }
        }
    }
    Msg = RED "ERROR: Channel does not exist\n" RESET;
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
}

void CommandHandler::processInvite(Client &client, Server &server, const std::vector<std::string> &args)
{
    (void)client;
    (void)server;
    (void)args;
    /* std::string Msg;
    if (args.size() < 2) {
        Msg = RED "ERROR: INVITE command requires a channel and a nickname: " RESET "INVITE <#channel> <nickname>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    std::string channel = args[0];
    std::string nick = args[1];

    // Buscar el canal y el cliente
    for (size_t i = 0; i < server.GetChannels().size(); ++i) {
        if (server.GetChannels()[i].GetName() == channel) {
            if (server.GetChannels()[i].isOperator(client.GetClientSocket())) {
                for (size_t j = 0; j < server.GetClients().size(); ++j) {
                    if (server.GetClients()[j].GetClientNick() == nick) {
                        server.GetChannels()[i].AddUser(server.GetClients()[j].GetClientSocket());
                        std::string Msg = GREEN "You have been invited to the channel\n" RESET;
                        send(server.GetClients()[j].GetClientSocket(), Msg.c_str(), Msg.size(), 0);
                        return;
                    }
                }
                std::string Msg = RED "ERROR: User does not exist\n" RESET;
                send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
                return;
            } else {
                std::string Msg = RED "ERROR: You are not an operator in this channel\n" RESET;
                send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
                return;
            }
        }
    }
    std::string Msg = RED "ERROR: Channel does not exist\n" RESET;
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0); */
}

void CommandHandler::processTopic(Client &client, Server &server, const std::vector<std::string> &args)
{
    (void)client;
    (void)server;
    (void)args;
    /* std::string Msg;
    if (args.size() < 2) {
        Msg = RED "ERROR: TOPIC command requires a channel and a topic: " RESET "TOPIC <#channel> <topic>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    std::string channel = args[0];
    std::string topic;

    // Construir el mensaje completo
    for (size_t i = 1; i < args.size(); ++i) {
        if (i > 1)
            topic += " ";
        topic += args[i];
    }

    // Buscar el canal
    for (size_t i = 0; i < server.GetChannels().size(); ++i) {
        if (server.GetChannels()[i].GetName() == channel) {
            if (server.GetChannels()[i].isOperator(client.GetClientSocket())) {
                server.GetChannels()[i].SetTopic(topic);
                std::string Msg = GREEN "Topic has been set\n" RESET;
                send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
                return;
            } else {
                std::string Msg = RED "ERROR: You are not an operator in this channel\n" RESET;
                send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
                return;
            }
        }
    }
    std::string Msg = RED "ERROR: Channel does not exist\n" RESET;
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0); */
}

void CommandHandler::processMode(Client &client, Server &server, const std::vector<std::string> &args)
{
    (void)client;
    (void)server;
    (void)args;
    /* std::string Msg;
    if (args.size() < 2) {
        Msg = RED "ERROR: MODE command requires a channel and a mode: " RESET "MODE <#channel> <mode>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    std::string channel = args[0];
    std::string mode = args[1];

    // Buscar el canal
    for (size_t i = 0; i < server.GetChannels().size(); ++i) {
        if (server.GetChannels()[i].GetName() == channel) {
            if (server.GetChannels()[i].isOperator(client.GetClientSocket())) {
                server.GetChannels()[i].SetMode(mode);
                std::string Msg = GREEN "Mode has been set\n" RESET;
                send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
                return;
            } else {
                std::string Msg = RED "ERROR: You are not an operator in this channel\n" RESET;
                send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
                return;
            }
        }
    }
    std::string Msg = RED "ERROR: Channel does not exist\n" RESET;
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0); */
}

// Funciones auxiliares

void CommandHandler::sendToChannel(Server &server, const std::string &channelName, const std::string &msg, Client &client)
{
    for (size_t i = 0; i < server.GetChannels().size(); ++i) {
        if (server.GetChannels()[i].GetName() == channelName) {
            std::vector<int> users = server.GetChannels()[i].GetUsers();
            bool found = false;
            for (size_t j = 0; j < users.size(); ++j) {
                if (users[j] == client.GetClientSocket()) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::string Msg = RED "ERROR: You are not in the channel\n" RESET;
                send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
                return;
            }
            for (size_t j = 0; j < users.size(); ++j) {
                if (users[j] != client.GetClientSocket()) {
                    std::string message = client.GetClientNick() + YELLOW " PRIVMSG " RESET + channelName + " :" + msg + "\n";
                    send(users[j], message.c_str(), message.size(), 0);
                }
            }
            return;
        }
    }
    std::string Msg = RED "ERROR: Channel does not exist\n" RESET;
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
}

void CommandHandler::sendToClient(Server &server, const std::string &clientNick, const std::string &msg, Client &client)
{
    for (size_t i = 0; i < server.GetClients().size(); ++i) {
        if (server.GetClients()[i].GetClientNick() == clientNick) {
            std::string message = client.GetClientNick() + YELLOW " PRIVMSG " RESET + clientNick + " :" + msg + "\n";
            send(server.GetClients()[i].GetClientSocket(), message.c_str(), message.size(), 0);
            return;
        }
    }
    std::string Msg = RED "ERROR: User does not exist\n" RESET;
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
}

bool CommandHandler::handleOperatorCommand(Client &client, Server &server, 
                                            const std::string &channelName, const std::string &command, 
                                            const std::vector<std::string> &args)
{
    for (size_t i = 0; i < server.GetChannels().size(); ++i) {
        if (server.GetChannels()[i].GetName() == channelName) {
            if (server.GetChannels()[i].isOperator(client.GetClientSocket())) {
                // Ejecutar comando con privilegios de operador
                if (command == "KICK") {
                    processKick(client, server, args);
                } /* else if (command == "INVITE") {
                    processInvite(client, server, args);
                } else if (command == "TOPIC") {
                    processTopic(client, server, args);
                } else if (command == "MODE") {
                    processMode(client, server, args);
                } */
                std::cout  << Server::getCurrentTime() 
                            << GREEN << "[+] Client <" << client.GetClientSocket() << "> executed operator command: " << MAGENTA << command << RESET << std::endl;
                return true;
            } else {
                std::string Msg = RED "ERROR: You are not an operator in this channel\n" RESET;
                send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
                return false;
            }   
        }
    }
    // Si el canal no existe, enviamos un mensaje de error
    std::string Msg = RED "ERROR: Channel does not exist\n" RESET;
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    return false;
}
