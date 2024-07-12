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
    } else if (command == "NICK" && client.GetAuthenticated() == true) {
        processNick(client, server, tokens);
    } else if (command == "USER" && client.GetAuthenticated() == true) {
        processUser(client, server, tokens);
    } else if (command == "JOIN" && client.GetAuthenticated() == true) {
        processJoin(client, server, tokens);
    } else if (command == "PRIVMSG" && client.GetAuthenticated() == true && client.GetClientNick() != ""){
        processPrivmsg(client, server, tokens);
    } else if ((command == "KICK" || command == "INVITE" || command == "TOPIC" || command == "MODE") && client.GetAuthenticated() == true && client.GetClientNick() != ""){
        if (!tokens.empty()) {
            //std::string channelName = tokens[0];
            handleOperatorCommand(client, server, command, tokens);
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

    if (args.size() < 1 || args[0] == "") {
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
            return;
        }
    }

    // Si el nick es válido, lo establecemos
    client.SetClientNick(nick);
    client.SetNickSet(true); // Indicamos que el nick ha sido establecido
    std::cout   << Server::getCurrentTime() 
                << GREEN << "[+] Client <" << client.GetClientSocket() << "> set nickname to " 
                << MAGENTA << nick << RESET << std::endl;
    if (client.getUser() == false || client.getNick() == false) {
        Msg = GREEN "Your Nickname has been set. Use USER command to continue\n" RESET;
        std::cout << "Nickset: " << client.getNick() << std::endl;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    } else {
        Msg = GREEN "Your Nickname has been set successfully.\n" RESET;
        std::cout << "Nickset: " << client.getNick() << std::endl;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    }
}

void CommandHandler::processUser(Client &client, Server &/*server*/, const std::vector<std::string> &args)
{
    std::string Msg;

    if (args.size() < 4) {
        Msg = RED "ERROR: USER command requires 4 arguments:\n" RESET "USER <username> <hostname> <servername> <realname>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // Actualizar la información del cliente
    client.SetUsername(args[0]);
    client.SetHostname(args[1]);
    client.SetServername(args[2]);
    client.SetRealname(args[3]);

    // Enviar confirmación al cliente
    client.SetUserSet(true);
    /* if (!client.IsFullyAuthenticated()) {
        Msg = GREEN "Your User information has been set. Use NICK command to continue\n" RESET;
    } else if(client.IsFullyAuthenticated()) {
        Msg = GREEN "Your User information has been set successfully.\n" RESET;
    } */
    if (client.getUser() == false || client.getNick() == false) {
        Msg = GREEN "Your User information has been set. Use NICK command to continue\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);

        std::cout << "Userset: " << client.getUser() << std::endl;
        return;
    }
    std::cout << "Userset: " << client.getUser() << std::endl;
    Msg = GREEN "Your User information has been set successfully.\n" RESET;
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);

}

void CommandHandler::processJoin(Client &client, Server &server, const std::vector<std::string> &args)
{
    std::string Msg;
    if (args.size() < 1){
        Msg = RED "ERROR: JOIN requires a channel and a password in case it is needed: " RESET "JOIN <#channel> [password]\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // Comprobar que USER y NICK fueron creados
    std::cout << "User: " << client.getUser() << std::endl;
    std::cout << "Nick: " << client.getNick() << std::endl;
    if (client.getUser() == false || client.getNick() == false) {
        std::string msg = RED "ERROR: You must set NICK and USER before joining a channel\n" RESET;
        send(client.GetClientSocket(), msg.c_str(), msg.size(), 0);
        return;
    } 

    if (args[1] == "") {
        Msg = RED "ERROR: NICK command requires a Nickname: " RESET "NICK <nickname>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    // Comprobar si el cliente ya está en un canal
    std::string channel = args[0];
    if (channel[0] != '#') {
        Msg = RED "ERROR: Channel name must start with '#': " RESET "JOIN <#channel>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // Usa el servidor para añadir el cliente al canal
    server.JoinChannel(client, channel, args);
}

void CommandHandler::processPrivmsg(Client &client, Server &server, const std::vector<std::string> &args)
{
    std::string Msg;
    if (args.size() < 2) {
        Msg = RED "ERROR: PRIVMSG command requires a nick or channel and a message: " RESET "PRIVMSG <nick or #channel> :<message>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    std::string destinatary = args[0];
    std::string message;

    // Construir el mensaje completo
    message = args[1];
    for (size_t i = 2; i < args.size(); ++i) {
        message += " ";
        message += args[i];
    }

    // Verificar si el mensaje tiene el formato correcto
    size_t pos = message.find(':');
    if (message[0] != ':') {
        Msg = RED "ERROR: Invalid message format. Use PRIVMSG <nick or #channel> :<message>\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    std::string msgContent = message.substr(pos + 1); // Obtener el contenido del mensaje

    //Buscar el destinatario
    if (destinatary[0] == '#') {
        sendToChannel(server, destinatary, msgContent, client);
    } else {
        sendToClient(server, destinatary, msgContent, client);
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

    //Determinar numero de socket del destinatario
    int destSocket = server.GetSocketByNick(nick);
    if (destSocket == -1) {
        Msg = RED "ERROR: User does not exist\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    Channel* channelObj = server.GetThisChannel(channel);
    //Determinar si el usuario esta en el canal y no es el mismo
    bool found = false;
    for (size_t i = 0; i < channelObj->GetUsers().size(); i++) {
        if (channelObj->GetUsers()[i] == destSocket && destSocket != client.GetClientSocket()) {
            found = true;
            break;
        }
    }
    if (!found) {
        if (destSocket == client.GetClientSocket()) {
            Msg = RED "ERROR: You can't kick yourself\n" RESET;
        } else {
            Msg = RED "ERROR: User does not exist in the channel\n" RESET;
        }
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    //Eliminar al usuario del canal
    channelObj->RemoveUser(destSocket);
    channelObj->removeOperator(destSocket);  //Eliminar de la lista de administradores en caso de serlo
    Msg = GREEN "User has been kicked from the channel\n" RESET;
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    Msg = RED "You have been kicked from the channel\n" RESET;
    send(destSocket, Msg.c_str(), Msg.size(), 0);
    return;
}

void CommandHandler::processInvite(Client &client, Server &server, const std::vector<std::string> &args)
{

    std::string Msg;
    if (args.size() < 2) {
        Msg = RED "ERROR: INVITE command requires a channel and a nickname: " RESET "INVITE <nickname> <#channel>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    std::string channel = args[1];
    std::string nick = args[0];

    //Determinar numero de socket del destinatario
    int destSocket = server.GetSocketByNick(nick);
    if (destSocket == -1) {
        Msg = RED "ERROR: User does not exist\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    //Determinar si el usuario esta en el canal
    Channel* channelObj = server.GetThisChannel(channel);
    bool found = false;
    for (size_t i = 0; i < channelObj->GetUsers().size(); i++) {
        if (channelObj->GetUsers()[i] == destSocket) {
            found = true;
            break;
        }
    }
    if (found) {
        Msg = RED "ERROR: User is already in the channel\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    //Invitar al usuario
    channelObj->inviteUser(destSocket);
    Msg = GREEN "User has been invited to the channel\n" RESET;
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    std::string InvitingUser = client.GetClientNick();
    Msg = GREEN "You have been invited to the channel " + channel + " by " + InvitingUser + "\n" RESET;
    send(destSocket, Msg.c_str(), Msg.size(), 0);
    return;

}

void CommandHandler::processTopic(Client &client, Server &server, const std::vector<std::string> &args)
{
    std::string Msg;

    if (args.size() < 1) {
        Msg = RED "ERROR: TOPIC command requires a channel: " RESET "TOPIC <#channel>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    std::string channel = args[0];
    std::string topic;

    // Construir el mensaje completo
    if (args.size() > 1){
        topic = args[1];
        for (size_t i = 2; i < args.size(); ++i) {
            topic += " ";
            topic += args[i];
        }
        if (topic[0] != ':') {
            Msg = RED "ERROR: Invalid topic format. Use TOPIC <#channel> :<topic>\n" RESET;
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
    }


    Channel* channelObj = server.GetThisChannel(channel);
    if (topic.empty()) {
        //Mostrar el topic almacenado en el canal
        Msg = GREEN "Topic: " + channelObj->GetName() + " : " + channelObj->getTopic() + "\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    //Establecer el topic
    //Comprobar si el usuario está en el canal
    if (!channelObj->UserExists(client.GetClientSocket())) {
        Msg = RED "ERROR: You can't set the topic of a channel you are not in\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    if (channelObj->isTopicblock()) {
        Msg = RED "ERROR: Topic can only be set by operators\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    channelObj->setTopic(topic);
    Msg = GREEN "Topic has been set\n" RESET;
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    return;
}

void CommandHandler::processMode(Client &client, Server &server, const std::vector<std::string> &args)
{
std::string Msg;
    if (args.size() < 2) {
        Msg = RED "ERROR: MODE command requires a channel and a mode: " RESET "MODE <#channel> <mode>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    std::string channel = args[0];
    std::string mode = args[1];

    Channel* channelObj = server.GetThisChannel(channel);

    //Establecer el modo
    if (mode == "+i") {
        channelObj->setInviteOnly(true);
        Msg = GREEN "Invite only mode has been set\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    } else if (mode == "-i") {
        channelObj->setInviteOnly(false);
        Msg = GREEN "Invite only mode has been removed\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    } else if (mode == "+t") {
        channelObj->setTopicblock(true);
        Msg = GREEN "Topic block mode has been set\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    } else if (mode == "-t") {
        channelObj->setTopicblock(false);
        Msg = GREEN "Topic block mode has been removed\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }  else if (mode == "+o" || mode == "-o") {
        if (args.size() < 3) {
            Msg = RED "ERROR: MODE command requires a channel, a mode and a nickname: " RESET "MODE <#channel> <mode> <nickname>\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        std::string nick = args[2];
        //Comprobar si el usuario destino está en el canal
        int destSocket = server.GetSocketByNick(nick);
        bool found = false;
        for (size_t i = 0; i < channelObj->GetUsers().size(); i++) {
            if (channelObj->GetUsers()[i] == destSocket) {
                found = true;
                break;
            }
        }
        if (!found) {
            Msg = RED "ERROR: User is not in the channel\n" RESET;
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        //Agregar el usuario como operador al canal
        if (mode == "+o") {
            channelObj->addOperator(destSocket);
            Msg = GREEN "User has been set as operator\n" RESET;
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            Msg = GREEN "You are now an operator of channel " + channel + "\n" RESET;
            send(destSocket, Msg.c_str(), Msg.size(), 0);
            return;
        } else {
            channelObj->removeOperator(destSocket);
            Msg = GREEN "User has been removed as operator\n" RESET;
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            Msg = GREEN "You are no longer an operator of channel " + channel + "\n" RESET;
            send(destSocket, Msg.c_str(), Msg.size(), 0);
            return;
        }
    }   else if (mode == "+k"){
        if (args.size() < 3) {
            Msg = RED "ERROR: MODE command requires a channel, a mode and a password: " RESET "MODE <#channel> <mode> <password>\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        std::string password = args[2];
        channelObj->setPassword(password);
        Msg = GREEN "Password has been set\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    } else if (mode == "-k"){
        channelObj->setPassword("");
        Msg = GREEN "Password has been removed\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    } else if (mode == "+l") {
        if (args.size() < 3) {
            Msg = RED "ERROR: MODE command requires a channel, a mode and a limit: " RESET "MODE <#channel> <mode> <limit>\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        unsigned int limit;
        std::stringstream ss(args[2]);
        if (!(ss >> limit) || isNumber(args[2]) == false){
            Msg = RED "ERROR: Invalid number\n" RESET;
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        channelObj->setLimitUsers(limit);
        channelObj->setLimitUsersEnabled(true);
        Msg = GREEN "User limit has been set\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    } else if (mode == "-l") {
        channelObj->setLimitUsers(0);
        channelObj->setLimitUsersEnabled(false);
        Msg = GREEN "User limit has been removed\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    } else {
        Msg = RED "ERROR: Invalid mode\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
  
    }

}

// Funciones auxiliares

void CommandHandler::sendToChannel(Server &server, const std::string &channelName, const std::string &msg, Client &client)
{
    Channel* channel = server.GetThisChannel(channelName);
    if (channel != nullptr) {
        //comprobar si el usuario esta en el canal
        if (!channel->UserExists(client.GetClientSocket())) {
            std::string Msg = RED "ERROR: You are not in the channel\n" RESET;
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        std::vector<int> users = channel->GetUsers();
        for (size_t i = 0; i < users.size(); i++) {
            if (users[i] != client.GetClientSocket()) {
                std::string message = "\n" + client.GetClientNick() + YELLOW " PRIVMSG " RESET + channelName + " :" + msg + "\n";
                send(users[i], message.c_str(), message.size(), 0);
            }
        }
        return;
    }
    std::string Msg = RED "ERROR: Channel does not exist\n" RESET;
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    return;
}

void CommandHandler::sendToClient(Server &server, const std::string &clientNick, const std::string &msg, Client &client)
{
    int detinatary = server.GetSocketByNick(clientNick);
    if (detinatary != -1) {
        if (detinatary == client.GetClientSocket()) {
            std::string Msg = RED "ERROR: You can't send a message to yourself\n" RESET;
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        std::string Msg = client.GetClientNick() + YELLOW " PRIVMSG " RESET + clientNick + " :" + msg + "\n";
        send(detinatary, Msg.c_str(), Msg.size(), 0);
        return;
    }
    std::string Msg = RED "ERROR: User does not exist\n" RESET;
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
}

bool CommandHandler::isNumber(const std::string& s)
{
    for (size_t i = 0; i < s.length(); ++i) {
        if (!isdigit(s[i])) {
            return false;
        }
    }
    return true;
}

bool CommandHandler::handleOperatorCommand(Client &client, Server &server, 
                                            const std::string &command, 
                                            const std::vector<std::string> &args)
{
    Channel* channel = nullptr;
    if (command == "INVITE"){
        channel = server.GetThisChannel(args[1]);
    } else {
        channel = server.GetThisChannel(args[0]);
    }

    if (channel == nullptr) {
        std::string Msg = RED "ERROR: Channel does not exist\n" RESET;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return false;
    }

    if (command == "TOPIC") {
        processTopic(client, server, args);
    }
    else {
        bool isOperator = channel->isOperator(client.GetClientSocket());
        if (isOperator) {
            if (command == "KICK") {
                processKick(client, server, args);
            } else if (command == "INVITE") {
                processInvite(client, server, args);
            }
            else if (command == "MODE") {
                processMode(client, server, args);
            }
        } else {
            std::string Msg = RED "ERROR: You are not an operator in this channel\n" RESET;
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return false;
        }
    }
    std::cout  << Server::getCurrentTime() 
                << GREEN << "[+] Client <" << client.GetClientSocket() << "> executed operator command: " << MAGENTA << command << RESET << std::endl;
    return true;
}
