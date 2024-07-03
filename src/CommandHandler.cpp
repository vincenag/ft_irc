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
    } /*else if (command == "USER") {
        processUser(client, tokens);
    }  else if (command == "KICK") {
        processKick(client, tokens);
    } else if (command == "INVITE") {
        processInvite(client, tokens);
    } else if (command == "TOPIC") {
        processTopic(client, tokens);
    } else if (command == "MODE") {
        processMode(client, tokens);
    } else {
        std::cout << "Unknown command: " << command << std::endl;
    } */
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
        std::cout << "Client <" << client.GetClientSocket() << "> authenticated" << std::endl;
        Msg = GREEN "You have been authenticated. Use NICK command to continue\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    } else {
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        std::cout << "Client <" << client.GetClientSocket() << "> failed to authenticate" << std::endl;
        server.RemoveClient(client.GetClientSocket());
        std::cout << "Client <" << client.GetClientSocket() << "> removed" << std::endl;
    }
}

void CommandHandler::processNick(Client &client, Server &server, const std::vector<std::string> &args)
{
    std::string Msg;

    if (args.size() < 1) {
        Msg = RED "ERROR: NICK command requires a Nickname\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    std::string nick = args[0];
    // Comprobar si el nick ya está en uso
    for (size_t i = 0; i < server.GetClients().size(); i++) {
        if (server.GetClients()[i].GetClientNick() == nick && server.GetClients()[i].GetClientSocket() != client.GetClientSocket()){
            Msg = RED "ERROR: Nickname already in use\n" RESET;
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            std::cout << "Client <" << client.GetClientSocket() << "> failed to set nickname" << std::endl;
            return;
        }
    }

    client.SetClientNick(nick);
    std::cout << "Client <" << client.GetClientSocket() << "> set nickname to " << nick << std::endl;
    Msg = GREEN "Your Nickname has been set\n" RESET;
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
}

void CommandHandler::processJoin(Client &client, Server &server, const std::vector<std::string> &args)
{
    std::string Msg;
    if (args.size() < 1) {
        Msg = RED "ERROR: JOIN command requires a channel\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    std::string channel = args[0];
    if (channel[0] != '#') {
        Msg = RED "ERROR: Channel name must start with '#'\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // Usa el servidor para añadir el cliente al canal
    server.JoinChannel(client, channel);
    std::cout << "Client <" << client.GetClientSocket() << "> joined channel " << channel << std::endl;
    Msg = GREEN "You have joined the channel\n" RESET;
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
}

void CommandHandler::processPrivmsg(Client &client, Server &server, const std::vector<std::string> &args)
{
    std::string Msg;
    if (args.size() < 2) {
        Msg = RED "ERROR: PRIVMSG command requires a channel and a message\n" RESET;
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
        Msg = RED "ERROR: Invalid message format. Use PRIVMSG <channel> :<message>\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    std::string msgContent = message.substr(pos + 1); // Obtener el contenido del mensaje

    // Buscar el canal
    for (size_t i = 0; i < server.GetChannels().size(); ++i) {
        if (server.GetChannels()[i].GetName() == channel) {
            // Verificar si el cliente pertenece al canal
            std::vector<int> users = server.GetChannels()[i].GetUsers();
            bool found = false;
            for (size_t j = 0; j < users.size(); ++j) {
                if (users[j] == client.GetClientSocket()) {
                    found = true;
                    break;
                }
            }
            // Enviar el mensaje a todos los usuarios del canal
            if (found) {
                for (size_t j = 0; j < users.size(); ++j) {
                    if (users[j] != client.GetClientSocket()) {
                        std::string msg = client.GetClientNick() + YELLOW " PRIVMSG " RESET + channel + " :" + msgContent + "\n";
                        send(users[j], msg.c_str(), msg.size(), 0);
                    }
                }
                return;
            }
        }
    }
    //Si el canal no existe
    Msg = RED "ERROR: Channel does not exist or you are not in the channel\n" RESET;
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
}