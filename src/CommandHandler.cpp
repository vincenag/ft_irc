#include "../inc/CommandHandler.hpp"

//CommandHandler::CommandHandler(Server& server) : server(server) {}

void CommandHandler::handleCommand(Client &client, const std::string &commandLine)
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
    if (command == "PASS") {
        processPass(client, tokens);
    } else if (command == "NICK") {
        processNick(client, tokens);
    } else if (command == "USER") {
        processUser(client, tokens);
    } else if (command == "JOIN") {
        processJoin(client, tokens);
    } else if (command == "PRIVMSG") {
        processPrivmsg(client, tokens);
    } else if (command == "KICK") {
        processKick(client, tokens);
    } else if (command == "INVITE") {
        processInvite(client, tokens);
    } else if (command == "TOPIC") {
        processTopic(client, tokens);
    } else if (command == "MODE") {
        processMode(client, tokens);
    } else {
        std::cout << "Unknown command: " << command << std::endl;
    }
}

std::vector<std::string> CommandHandler::splitCommand(const std::string &commandLine)
{
    std::vector<std::string> tokens;
    std::istringstream stream(commandLine);
    std::string token;
    while (std::getline(stream, token, ' ')) {
        tokens.push_back(token);
    }
    return tokens;
}

void CommandHandler::processNick(Client& client, const std::vector<std::string>& args) 
{
    if (args.size() < 1) {
        std::cerr << "NICK command requires a nickname" << std::endl;
        return;
    }
    std::string nickname = args[0];
    // Usa el servidor para verificar si el nickname ya está en uso
    if (server.isNicknameTaken(nickname)) {
        std::cerr << "Nickname " << nickname << " is already in use" << std::endl;
        return;
    }
    client.setNickname(nickname);
    std::cout << "Client " << client.GetClientSocket() << " set nickname to " << nickname << std::endl;
}

void CommandHandler::processUser(Client& client, const std::vector<std::string>& args) 
{
    if (args.size() < 1) {
        std::cerr << "USER command requires a username" << std::endl;
        return;
    }
    std::string username = args[0];
    client.setNickname(username);
    std::cout << "Client " << client.GetClientSocket() << " set username to " << username << std::endl;
}

void CommandHandler::processJoin(Client& client, const std::vector<std::string>& args) 
{
    if (args.size() < 1) {
        std::cerr << "JOIN command requires a channel" << std::endl;
        return;
    }
    std::string channel = args[0];
    // Usa el servidor para añadir el cliente al canal
    server.joinChannel(channel, client);
    std::cout << "Client " << client.GetClientSocket() << " joined channel " << channel << std::endl;
}

void CommandHandler::processPrivmsg(Client& client, const std::vector<std::string>& args) 
{
    if (args.size() < 2) {
        std::cerr << "PRIVMSG command requires a target and a message" << std::endl;
        return;
    }
    std::string target = args[0];
    std::string message = args[1];
    // Usa el servidor para enviar el mensaje al target
    server.sendMessage(target, message, client);
    std::cout << "Client " << client.GetClientSocket() << " sent message to " << target << ": " << message << std::endl;
}

void CommandHandler::processKick(Client& client, const std::vector<std::string>& args) 
{
    if (args.size() < 2) {
        std::cerr << "KICK command requires a channel and a user" << std::endl;
        return;
    }
    std::string channel = args[0];
    std::string user = args[1];
    // Usa el servidor para expulsar al usuario del canal
    server.kickUser(channel, user, client);
    std::cout << "Client " << client.GetClientSocket() << " kicked user " << user << " from channel " << channel << std::endl;
}

void CommandHandler::processInvite(Client& client, const std::vector<std::string>& args) 
{
    if (args.size() < 2) {
        std::cerr << "INVITE command requires a user and a channel" << std::endl;
        return;
    }
    std::string user = args[0];
    std::string channel = args[1];
    // Usa el servidor para invitar al usuario al canal
    server.inviteUser(user, channel, client);
    std::cout << "Client " << client.GetClientSocket() << " invited user " << user << " to channel " << channel << std::endl;
}

void CommandHandler::processTopic(Client& client, const std::vector<std::string>& args) 
{
    if (args.size() < 1) {
        std::cerr << "TOPIC command requires a topic" << std::endl;
        return;
    }

    std::string channel = args[0];
    std::string topic = args.size() > 1 ? args[1] : "";
    // Usa el servidor para establecer o obtener el tema del canal
    server.setTopic(channel, topic, client);
    std::cout << "Client " << client.GetClientSocket() << " set topic to " << topic << std::endl;
}

void CommandHandler::processMode(Client& client, const std::vector<std::string>& args) 
{
    if (args.size() < 1) {
        std::cerr << "MODE command requires a mode" << std::endl;
        return;
    }

    std::string channel = args[0];
    std::string mode = args[0];
    // Usa el servidor para cambiar el modo del canal
    server.setChannelMode(channel, mode, client);
    std::cout << "Client " << client.GetClientSocket() << " set mode to " << mode << std::endl;
}
