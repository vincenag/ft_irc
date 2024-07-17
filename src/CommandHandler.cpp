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
    if (command == "PASS"){
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
            std::string Msg =  "ERROR: Command requires a channel\n" ;
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        }
    } else if (command != "CAP") {
        std::string msg =  "ERROR: Unknown command\n" ;
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
    // ERR_NEEDMOREPARAMS (461)
    if (args.empty()) {
        Utiles::sendNumericReply(client, 461, "PASS :Not enough parameters");
        server.RemoveClient(client.GetClientSocket());
        return;
    }

    // ERR_ALREADYREGISTERED (462)
    if (client.GetAuthenticated()) {
        Utiles::sendNumericReply(client, 462, ":You're already registered");
        return;
    }

    std::string password = args[0];
    if (server.GetPassword() == password) {
        client.SetAuthenticated(true);
    } else {
        // ERR_PASSWDMISMATCH (464)
        Utiles::sendNumericReply(client, 464, ":Password incorrect");
        server.RemoveClient(client.GetClientSocket());
    }
}

void CommandHandler::processNick(Client &client, Server &server, const std::vector<std::string> &args)
{
    /* std::string Msg;

    if (args.size() < 1 || args[0] == "") {
        Msg =  "ERROR: NICK command requires a Nickname: "  "NICK <nickname>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    std::string nick = args[0];

    // Si el cliente ya tiene este nick, enviamos un mensaje de error
    if (client.GetClientNick() == nick) {
        Msg =  "ERROR: Nickname already set\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // Comprobar si el nick ya está en uso en el server
    for (size_t i = 0; i < server.GetClients().size(); i++) {
        if (server.GetClients()[i].GetClientNick() == nick && server.GetClients()[i].GetClientSocket() != client.GetClientSocket()){
            Msg =  "ERROR: Nickname already in use\n";
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
        Msg =  "Your Nickname has been set. Use USER command to continue\n";
        std::cout << "Nickset: " << client.getNick() << std::endl;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    } else {
        Msg = "Your Nickname has been set successfully.\n";
        std::cout << "Nickset: " << client.getNick() << std::endl;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    } */

    std::string Msg;
    std::string serverName = "ft_irc"; // Cambia esto por el nombre de tu servidor
    std::string nick = args.size() > 0 ? args[0] : "";

    // Comprobar si se ha proporcionado un nick
    if (nick.empty()) {
        Msg = ":" + serverName + " 431 * :No nickname given\n"; // ERR_NONICKNAMEGIVEN
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // Si el cliente ya tiene este nick, enviamos un mensaje de error
    if (client.GetClientNick() == nick) {
        Msg = ":" + serverName + " 433 " + nick + " :Nickname is already in use\n"; // ERR_NICKNAMEINUSE
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // Comprobar si el nick ya está en uso en el servidor
    for (size_t i = 0; i < server.GetClients().size(); i++) {
        if (server.GetClients()[i].GetClientNick() == nick && server.GetClients()[i].GetClientSocket() != client.GetClientSocket()) {
            Msg = ":" + serverName + " 433 " + nick + " :Nickname is already in use\n"; // ERR_NICKNAMEINUSE
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
    }

    // Si el nick es válido, lo establecemos
    client.SetClientNick(nick);
    client.SetNickSet(true); // Indicamos que el nick ha sido establecido
    std::cout << Server::getCurrentTime() 
              << GREEN << "[+] Client <" << client.GetClientSocket() << "> set nickname to " 
              << MAGENTA << nick << RESET << std::endl;

    // Verificar si el usuario ya ha proporcionado USER y NICK
    if (client.getUser()) {
        // Mensajes de bienvenida
        Msg = ":" + serverName + " 001 " + nick + " :Welcome to the IRC network, " + nick + "\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);

        Msg = ":" + serverName + " 002 " + nick + " :Your host is " + serverName + ", running version 1.0\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);

        Msg = ":" + serverName + " 003 " + nick + " :This server was created today\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);

        Msg = ":" + serverName + " 004 " + nick + " " + serverName + " 1.0 o o\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    } else {
        // Mensaje para completar el comando USER
        Msg = "Your Nickname has been set. Use USER command to continue\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    }
}

void CommandHandler::processUser(Client &client, Server &/*server*/, const std::vector<std::string> &args)
{
    /* std::string Msg;

    if (args.size() < 4) {
        Msg = "ERROR: USER command requires 4 arguments:\n" "USER <username> <hostname> <servername> <realname>\n";
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
    if (client.getUser() == false || client.getNick() == false) {
        Msg = "Your User information has been set. Use NICK command to continue\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    Msg = "Your User information has been set successfully.\n";
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0); */

    if (client.IsRegistered()) {
        // 462 es el código de error para "Ya estás registrado"
        Utiles::sendNumericReply(client, 462, "You're already registered");
        return;
    }

    if (args.size() < 4) {
        // 461 es el código de error para "Necesitas más parámetros"
        Utiles::sendNumericReply(client, 461, "USER :Not enough parameters");
        return;
    }

    // Actualizar la información del cliente
    client.SetUsername(args[0]);
    client.SetHostname(args[1]);
    client.SetServername(args[2]);
    client.SetRealname(args[3]);
    client.SetUserSet(true);

    // Verificar si el cliente ya ha establecido su NICK y está completamente registrado
    if (client.getNick() && !client.IsRegistered()) {
        // Marcar al cliente como registrado
        client.SetRegistered(true);

        // Enviar mensajes de bienvenida y MOTD
        Utiles::sendWelcomeMessage(client);
    } else {
        // Informar al cliente que debe usar el comando NICK para completar el registro
        Utiles::sendNumericReply(client, -1, "Your User information has been set. Use NICK command to complete registration.");
    }

}

void CommandHandler::processJoin(Client &client, Server &server, const std::vector<std::string> &args)
{
    /* std::string Msg;
    if (args.size() < 1){
        Msg =  "ERROR: JOIN requires a channel and a password in case it is needed: " "JOIN <#channel> [password]\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // Comprobar que USER y NICK fueron creados
    std::cout << "User: " << client.getUser() << std::endl;
    std::cout << "Nick: " << client.getNick() << std::endl;
    if (client.getUser() == false || client.getNick() == false) {
        std::string msg = "ERROR: You must set NICK and USER before joining a channel\n";
        send(client.GetClientSocket(), msg.c_str(), msg.size(), 0);
        return;
    } 

    if (args[1] == "") {
        Msg = "ERROR: NICK command requires a Nickname: " "NICK <nickname>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    // Comprobar si el cliente ya está en un canal
    std::string channel = args[0];
    if (channel[0] != '#') {
        Msg = "ERROR: Channel name must start with '#': " "JOIN <#channel>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // Usa el servidor para añadir el cliente al canal
    server.JoinChannel(client, channel, args); */

    // Verificar se se proporcionó el nombre del canal
    std::string Msg;
    if (args.size() < 1){
        Msg = "ERROR: JOIN requires a channel: JOIN <#channel>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // Comprobar que USER y NICK fueron creados
    if (client.getUser() == false || client.getNick() == false) {
        Msg = "ERROR: You must set NICK and USER before joining a channel\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // Verficar que el nombre del canal comience con #
    std::string channel = args[0];
    if (channel[0] != '#') {
        Msg = "ERROR: Channel name must start with '#': JOIN <#channel>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // LLamar al servidor para añadir el cliente al canal
    server.JoinChannel(client, channel, args);

    // Mensaje de confirmación al cliente
    Msg = ":ft_irc 332 " + client.GetClientNick() + " " + channel + " :Welcome to " + channel + "\n";
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);

    // Enviar la lista de usuarios en el canal
    Channel* channelObj = server.GetThisChannel(channel);
    std::vector<int> users = channelObj->GetUsers();
    std::string userList = "= " + channel + " :";
    for (size_t i = 0; i < users.size(); i++) {
        Client& user = server.GetClientBySocket(users[i]);
        userList += user.GetClientNick() + " ";
    }
    userList += "\n";

    /**
     * ! Este codigo se estaba repitiendo dos veces al creal el canal, ya esta en server
     */
    /* // Enviar la lista de usuarios en el canal
    Msg = ":ft_irc 353 " + client.GetClientNick() + " " + userList;
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);

    // Enviar mensaje de fin de lista
    Msg = ":ft_irc 366 " + client.GetClientNick() + " " + channel + "\n";
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0); */
}

void CommandHandler::processPrivmsg(Client &client, Server &server, const std::vector<std::string> &args)
{
    /* std::string Msg;
    if (args.size() < 2) {
        Msg = "ERROR: PRIVMSG command requires a nick or channel and a message: " "PRIVMSG <nick or #channel> :<message>\n";
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
        Msg = "ERROR: Invalid message format. Use PRIVMSG <nick or #channel> :<message>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    std::string msgContent = message.substr(pos + 1); // Obtener el contenido del mensaje

    //Buscar el destinatario
    if (destinatary[0] == '#') {
        sendToChannel(server, destinatary, msgContent, client);
        Msg = "Message sent to channel\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    } else {
        sendToClient(server, destinatary, msgContent, client);
        Msg = "Message sent to user\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    } */

   if (args.size() < 2) {
        std::string Msg = "ERROR: PRIVMSG command requires a nick or channel and a message: PRIVMSG <nick or #channel> :<message>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    std::string destinatary = args[0];
    std::string message = args[1];
    for (size_t i = 2; i < args.size(); ++i) {
        message += " " + args[i];
    }

    // Asumiendo que tienes métodos para verificar si el destinatario es un usuario o un canal
    if (server.ChannelExists(destinatary)) {
        // Si es un canal, reenvía el mensaje a todos los usuarios en el canal
        Channel* channel = server.GetThisChannel(destinatary);
        if (channel) {
            std::string fullMessage = ":" + client.GetClientNick() + " PRIVMSG " + destinatary + " :" + message + "\n";
            server.broadcastToChannel(*channel, fullMessage, client.GetClientSocket());
        }
    } else if (server.isUser(destinatary)) {
        // Si es un usuario, envía el mensaje solo a ese usuario
        Client* recipient = server.GetUserByNick(destinatary);
        std::string fullMessage = ":" + client.GetClientNick() + " PRIVMSG " + destinatary + " :" + message + "\n";
        send(recipient->GetClientSocket(), fullMessage.c_str(), fullMessage.size(), 0);
    } else {
        // Destinatario no encontrado
        std::string errMsg = "ERROR: No such nick/channel\n";
        send(client.GetClientSocket(), errMsg.c_str(), errMsg.size(), 0);
    }
    
}

void CommandHandler::processKick(Client &client, Server &server, const std::vector<std::string> &args)
{
    /* std::string Msg;
    if (args.size() < 2) {
        Msg = "ERROR: KICK command requires a channel and a nickname: " "KICK <#channel> <nickname>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    std::string channel = args[0];
    std::string nick = args[1];

    //Determinar numero de socket del destinatario
    int destSocket = server.GetSocketByNick(nick);
    if (destSocket == -1) {
        Msg = "ERROR: User does not exist\n";
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
            Msg = "ERROR: You can't kick yourself\n";
        } else {
            Msg = "ERROR: User does not exist in the channel\n";
        }
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    //Eliminar al usuario del canal
    channelObj->RemoveUser(destSocket);
    channelObj->removeOperator(destSocket);  //Eliminar de la lista de administradores en caso de serlo
    Msg = "User has been kicked from the channel\n";
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    Msg = "You have been kicked from the channel\n";
    send(destSocket, Msg.c_str(), Msg.size(), 0);
    return; */

    // Verificar si el comando tiene los argumentos necesarios
    std::string Msg;
    std::string serverName = "ft_irc"; // Cambia esto por el nombre de tu servidor

    if (args.size() < 2) {
        Msg = ":" + serverName + " 461 " + client.GetClientNick() + " KICK :Not enough parameters\n"; // ERR_NEEDMOREPARAMS
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    
    std::string channelName = args[0];
    std::string nick = args[1];
    std::string reason = args.size() > 2 ? args[2] : "No reason provided";

    // Verificar si el canal existe
    Channel* channel = server.GetThisChannel(channelName);
    if (!channel) {
        Msg = ":" + serverName + " 403 " + client.GetClientNick() + " " + channelName + " :No such channel\n"; // ERR_NOSUCHCHANNEL
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // Verificar si el cliente es operador del canal
    if (!channel->IsOperator(client.GetClientSocket())) {
        Msg = ":" + serverName + " 482 " + client.GetClientNick() + " " + channelName + " :You're not channel operator\n"; // ERR_CHANOPRIVSNEEDED
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // Determinar número de socket del destinatario
    int destSocket = server.GetSocketByNick(nick);
    if (destSocket == -1) {
        Msg = ":" + serverName + " 401 " + client.GetClientNick() + " " + nick + " :No such nick/channel\n"; // ERR_NOSUCHNICK
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // Verificar si el usuario a ser expulsado está en el canal
    if (!channel->IsUserInChannel(destSocket)) {
        Msg = ":" + serverName + " 441 " + client.GetClientNick() + " " + nick + " " + channelName + " :They aren't on that channel\n"; // ERR_USERNOTINCHANNEL
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // Expulsar al usuario del canal
    channel->RemoveUser(destSocket);
    channel->removeOperator(destSocket);  // Eliminar de la lista de administradores en caso de serlo

    // Notificar a todos los usuarios del canal sobre la expulsión
    Msg = ":" + client.GetClientNick() + "!" + client.GetUsername() + "@" + client.GetHostname() + " KICK " + channelName + " " + nick + " :" + reason + "\n";
    std::vector<int> users = channel->GetUsers();
    for (size_t i = 0; i < users.size(); ++i) {
        send(users[i], Msg.c_str(), Msg.size(), 0);
    }

    // Notificar al usuario expulsado
    std::string kickMsg = ":" + serverName + " 474 " + nick + " " + channelName + " :You have been kicked from the channel. Reason: " + reason + "\n"; // RPL_KICKED
    send(destSocket, kickMsg.c_str(), kickMsg.size(), 0);
}

void CommandHandler::processInvite(Client &client, Server &server, const std::vector<std::string> &args)
{

    /* std::string Msg;
    if (args.size() < 2) {
        Msg =  "ERROR: INVITE command requires a channel and a nickname: "  "INVITE <nickname> <#channel>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    std::string channel = args[1];
    std::string nick = args[0];

    //Determinar numero de socket del destinatario
    int destSocket = server.GetSocketByNick(nick);
    if (destSocket == -1) {
        Msg =  "ERROR: User does not exist\n" ;
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
        Msg =  "ERROR: User is already in the channel\n" ;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    //Invitar al usuario
    channelObj->inviteUser(destSocket);
    Msg =  "User has been invited to the channel\n" ;
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    std::string InvitingUser = client.GetClientNick();
    Msg =  "You have been invited to the channel " + channel + " by " + InvitingUser + "\n" ;
    send(destSocket, Msg.c_str(), Msg.size(), 0);
    return; */

    if (args.size() < 2) {
        Utiles::sendNumericReply(client, 461, "INVITE :Not enough parameters");
        return;
    }

    std::string nick = args[0];
    std::string channel = args[1];

    // Verificar si el canal existe
    Channel* channelObj = server.GetThisChannel(channel);
    if (!channelObj) {
        Utiles::sendNumericReply(client, 403, channel + " :No such channel");
        return;
    }

    // Verificar si el cliente es miembro del canal
    if (!channelObj->IsUserInChannel(client.GetClientSocket())) {
        Utiles::sendNumericReply(client, 442, channel + " :You're not on that channel");
        return;
    }

    // Verificar si el destinatario existe
    int destSocket = server.GetSocketByNick(nick);
    if (destSocket == -1) {
        Utiles::sendNumericReply(client, 401, nick + " :No such nick/channel");
        return;
    }

    // Verificar si el destinatario ya está en el canal
    if (channelObj->IsUserInChannel(destSocket)) {
        Utiles::sendNumericReply(client, 443, nick + " " + channel + " :is already on channel");
        return;
    }

    // Enviar la invitación
    std::string inviteMsg = ":" + client.GetClientNick() + " INVITE " + nick + " :" + channel + "\r\n";    send(destSocket, inviteMsg.c_str(), inviteMsg.size(), 0);
    Utiles::sendNumericReply(client, 341, nick + " " + channel);
}

void CommandHandler::processTopic(Client &client, Server &server, const std::vector<std::string> &args)
{
    /* std::string Msg;

    if (args.size() < 1) {
        Msg =  "ERROR: TOPIC command requires a channel: "  "TOPIC <#channel>\n";
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
            Msg =  "ERROR: Invalid topic format. Use TOPIC <#channel> :<topic>\n" ;
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
    }


    Channel* channelObj = server.GetThisChannel(channel);
    if (topic.empty()) {
        //Mostrar el topic almacenado en el canal
        Msg =  "Topic: " + channelObj->GetName() + " : " + channelObj->getTopic() + "\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    //Establecer el topic
    //Comprobar si el usuario está en el canal
    if (!channelObj->UserExists(client.GetClientSocket())) {
        Msg = "ERROR: You can't set the topic of a channel you are not in\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    if (channelObj->isTopicblock()) {
        Msg = "ERROR: Topic can only be set by operators\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    channelObj->setTopic(topic);
    Msg = "Topic has been set\n";
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    return; */

    // Segun protocolo
    if (args.size() < 1) {
        // Enviar error si no se proporciona el nombre del canal
        Utiles::sendNumericReply(client, 461, "TOPIC :Not enough parameters");
        return;
    }

    std::string channelName = args[0];
    Channel* channel = server.GetThisChannel(channelName); // Asumiendo que existe este método en Server

    if (channel == nullptr) {
        // Enviar error si el canal no existe
        Utiles::sendNumericReply(client, 403, channelName + " :No such channel");
        return;
    }

    if (args.size() == 1) {
        // Si solo se proporciona el nombre del canal, devolver el tema actual
        std::string currentTopic = channel->getTopic();
        if (currentTopic.empty()) {
            Utiles::sendNumericReply(client, 331, channelName + " :No topic is set");
        } else {
            Utiles::sendNumericReply(client, 332, client.GetClientNick() + " " + channelName + " :" + currentTopic);
        }
    } else {
        // Verificar si el canal tiene el modo +t activado
        bool isTopicRestricted = channel->isModeSet('t');
        bool isOperator = channel->IsOperator(client.GetClientSocket());

        if (isTopicRestricted && !isOperator) {
            // Si el modo +t está activado y el cliente no es operador, enviar error
            Utiles::sendNumericReply(client, 482, channelName + " :You're not channel operator");
            return;
        }

        // Cambiar el tema si el cliente tiene permiso
        std::string newTopic;
        for (size_t i = 1; i < args.size(); ++i) {
            if (i > 1) newTopic += " ";
            newTopic += args[i];
        }

        channel->SetTopic(newTopic);
        std::string msg = ":" + client.GetClientNick() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
        channel->BroadcastMessage(msg, server);
    }
}

void CommandHandler::processMode(Client &client, Server &server, const std::vector<std::string> &args)
{
    // Codigo normal sin protocolo, pero actualiza la lista de usuarios en el canal
    // cuando el operador cambia el modo de +o o -o
    std::string Msg;
    if (args.size() < 2) {
        Msg = "ERROR: MODE command requires a channel and a mode: " "MODE <#channel> <mode>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    std::string channel = args[0];
    std::string mode = args[1];

    Channel* channelObj = server.GetThisChannel(channel);

    //Establecer el modo
    if (mode == "+i") {
        channelObj->setInviteOnly(true);
        Msg = "Invite only mode has been set\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    } else if (mode == "-i") {
        channelObj->setInviteOnly(false);
        Msg = "Invite only mode has been removed\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    } else if (mode == "+t") {
        channelObj->setTopicblock(true);
        Msg = "Topic block mode has been set\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    } else if (mode == "-t") {
        channelObj->setTopicblock(false);
        Msg = "Topic block mode has been removed\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }  else if (mode == "+o" || mode == "-o") {
        if (args.size() < 3) {
            Msg = "ERROR: MODE command requires a channel, a mode and a nickname: " "MODE <#channel> <mode> <nickname>\n";
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
            Msg = "ERROR: User is not in the channel\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        //Agregar el usuario como operador al canal
        if (mode == "+o") {
            channelObj->SetOperator(destSocket, true);
            Msg = "User has been set as operator\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            Msg = "You are now an operator of channel " + channel + "\n";
            send(destSocket, Msg.c_str(), Msg.size(), 0);
            // Actualizar la lista de usuarios en el canal (353)
            server.updateUserList(*channelObj);
            return;
        } else if (mode == "-o") {
            channelObj->SetOperator(destSocket, false);
            Msg = "User has been removed as operator\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            Msg = "You are no longer an operator of channel " + channel + "\n";
            send(destSocket, Msg.c_str(), Msg.size(), 0);
            // Actualizar la lista de usuarios en el canal (353)
            server.updateUserList(*channelObj);
            return;
        } else {
            Msg = "ERROR: Invalid mode\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
    }   else if (mode == "+k"){
        if (args.size() < 3) {
            Msg = "ERROR: MODE command requires a channel, a mode and a password: " "MODE <#channel> <mode> <password>\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        std::string password = args[2];
        channelObj->setPassword(password);
        Msg = "Password has been set\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    } else if (mode == "-k"){
        channelObj->setPassword("");
        Msg = "Password has been removed\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    } else if (mode == "+l") {
        if (args.size() < 3) {
            Msg = "ERROR: MODE command requires a channel, a mode and a limit: " "MODE <#channel> <mode> <limit>\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        unsigned int limit;
        std::stringstream ss(args[2]);
        if (!(ss >> limit) || isNumber(args[2]) == false){
            Msg = "ERROR: Invalid number\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        channelObj->setLimitUsers(limit);
        channelObj->setLimitUsersEnabled(true);
        Msg = "User limit has been set\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    } else if (mode == "-l") {
        channelObj->setLimitUsers(0);
        channelObj->setLimitUsersEnabled(false);
        Msg = "User limit has been removed\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    } else {
        Msg = "ERROR: Invalid mode\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    /* // Codigo con el protocolo, falta revisar bien, hace cosas raras
    std::string Msg;
    if (args.size() < 2) {
        Msg = ":server 461 " + client.GetClientNick() + " MODE :Not enough parameters\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }
    std::string channel = args[0];
    std::string mode = args[1];

    Channel* channelObj = server.GetThisChannel(channel);

    if (!channelObj) {
        Msg = ":server 403 " + client.GetClientNick() + " " + channel + " :No such channel\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    if (mode == "+i") {
        channelObj->setInviteOnly(true);
        Msg = ":server 324 " + client.GetClientNick() + " " + channel + " +i\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    } else if (mode == "-i") {
        channelObj->setInviteOnly(false);
        Msg = ":server 324 " + client.GetClientNick() + " " + channel + " -i\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    } else if (mode == "+t") {
        channelObj->setTopicblock(true);
        Msg = ":server 324 " + client.GetClientNick() + " " + channel + " +t\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    } else if (mode == "-t") {
        channelObj->setTopicblock(false);
        Msg = ":server 324 " + client.GetClientNick() + " " + channel + " -t\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    } else if (mode == "+o" || mode == "-o") {
        if (args.size() < 3) {
            Msg = ":server 461 " + client.GetClientNick() + " MODE :Not enough parameters\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        std::string nick = args[2];
        int destSocket = server.GetSocketByNick(nick);
        bool found = false;
        for (size_t i = 0; i < channelObj->GetUsers().size(); i++) {
            if (channelObj->GetUsers()[i] == destSocket) {
                found = true;
                break;
            }
        }
        if (!found) {
            Msg = ":server 441 " + client.GetClientNick() + " " + nick + " " + channel + " :They aren't on that channel\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        if (mode == "+o") {
            channelObj->SetOperator(destSocket, true);
            Msg = ":server 324 " + client.GetClientNick() + " " + channel + " +o " + nick + "\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            Msg = ":server 381 " + nick + " :You are now an operator of channel " + channel + "\n";
            send(destSocket, Msg.c_str(), Msg.size(), 0);
            server.updateUserList(*channelObj);
        } else if (mode == "-o") {
            channelObj->SetOperator(destSocket, false);
            Msg = ":server 324 " + client.GetClientNick() + " " + channel + " -o " + nick + "\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            Msg = ":server 482 " + nick + " :You are no longer an operator of channel " + channel + "\n";
            send(destSocket, Msg.c_str(), Msg.size(), 0);
            server.updateUserList(*channelObj);
        }
    } else if (mode == "+k") {
        if (args.size() < 3) {
            Msg = ":server 461 " + client.GetClientNick() + " MODE :Not enough parameters\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        std::string password = args[2];
        channelObj->setPassword(password);
        Msg = ":server 324 " + client.GetClientNick() + " " + channel + " +k\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    } else if (mode == "-k") {
        channelObj->setPassword("");
        Msg = ":server 324 " + client.GetClientNick() + " " + channel + " -k\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    } else if (mode == "+l") {
        if (args.size() < 3) {
            Msg = ":server 461 " + client.GetClientNick() + " MODE :Not enough parameters\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        unsigned int limit;
        std::stringstream ss(args[2]);
        if (!(ss >> limit) || !isNumber(args[2])) {
            Msg = ":server 461 " + client.GetClientNick() + " MODE :Invalid number\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        channelObj->setLimitUsers(limit);
        channelObj->setLimitUsersEnabled(true);
        Msg = ":server 324 " + client.GetClientNick() + " " + channel + " +l " + args[2] + "\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    } else if (mode == "-l") {
        channelObj->setLimitUsers(0);
        channelObj->setLimitUsersEnabled(false);
        Msg = ":server 324 " + client.GetClientNick() + " " + channel + " -l\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    } else {
        Msg = ":server 472 " + client.GetClientNick() + " " + mode + " :is unknown mode char to me\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    } */
}

// Funciones auxiliares

void CommandHandler::sendToChannel(Server &server, const std::string &channelName, const std::string &msg, Client &client)
{
    /* Channel* channel = server.GetThisChannel(channelName);
    if (channel != nullptr) {
        // Comprobar si el usuario está en el canal
        if (!channel->UserExists(client.GetClientSocket())) {
            std::string Msg = "ERROR: You are not in the channel\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        std::vector<int> users = channel->GetUsers();
        for (size_t i = 0; i < users.size(); i++) {
            if (users[i] != client.GetClientSocket()) {
                // Incluir el contenido de `msg` en el mensaje enviado
                std::string message = "\n" + client.GetClientNick() + " PRIVMSG " + channelName + " :" + msg + "\n";
                send(users[i], message.c_str(), message.size(), 0);
            }
        }
        return;
    }
    std::string Msg = "ERROR: Channel does not exist\n";
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    return; */

    // Funcion con protocolo implementada
    Channel* channel = server.GetThisChannel(channelName);
    if (channel == NULL) {
        std::string errorMsg = "ERROR :No such channel\r\n";
        send(client.GetClientSocket(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }

    if (!channel->UserExists(client.GetClientSocket())) {
        std::string errorMsg = "ERROR :You're not in that channel\r\n";
        send(client.GetClientSocket(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }

    std::string message = ":" + client.GetClientNick() + "!~" + client.GetUsername() + "@" + client.GetHostname() + " PRIVMSG " + channelName + " :" + msg + "\r\n";

    std::vector<int> users = channel->GetUsers();
    for (std::vector<int>::iterator it = users.begin(); it != users.end(); ++it) {
        if (*it != client.GetClientSocket()) {
            send(*it, message.c_str(), message.size(), 0);
        }
    }
}

void CommandHandler::sendToClient(Server &server, const std::string &clientNick, const std::string &msg, Client &client)
{
    int detinatary = server.GetSocketByNick(clientNick);
    if (detinatary != -1) {
        if (detinatary == client.GetClientSocket()) {
            std::string Msg = "ERROR: You can't send a message to yourself\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        std::string Msg = client.GetClientNick() + " PRIVMSG " + clientNick + " :" + msg + "\n";
        send(detinatary, Msg.c_str(), Msg.size(), 0);
        return;
    }
    std::string Msg = "ERROR: User does not exist\n";
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
        std::string Msg = "ERROR: Channel does not exist\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return false;
    }

    if (command == "TOPIC") {
        processTopic(client, server, args);
    }
    else {
        bool isOperator = channel->IsOperator(client.GetClientSocket());
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
            std::string Msg = "ERROR: You are not an operator in this channel\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return false;
        }
    }
    std::cout  << Server::getCurrentTime() 
                << GREEN << "[+] Client <" << client.GetClientSocket() << "> executed operator command: " << MAGENTA << command << RESET << std::endl;
    return true;
}
