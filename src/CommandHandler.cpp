#include "Library.hpp"

void CommandHandler::handleCommand(const std::string &commandLine, Server &server, Client &client)
{
    std::vector<std::string> tokens = splitCommand(commandLine);

    if (tokens.empty())
        return;

    std::string command = tokens[0];
    tokens.erase(tokens.begin());

    if (tokens[0] == HEXCHAT_SERVER) {
        tokens.erase(tokens.begin());
    }


    if (command == "PASS") {
        processPass(client, server, tokens);
    } else if (command == "NICK" && client.GetAuthenticated() == true) {
        processNick(client, server, tokens);
    } else if (command == "USER" && client.GetAuthenticated() == true) {
        processUser(client, server, tokens);
    } else if (command == "JOIN" && client.GetAuthenticated() == true) {
        processJoin(client, server, tokens);
    } else if (command == "PRIVMSG" && client.GetAuthenticated() == true && client.GetClientNick() != "") {
        processPrivmsg(client, server, tokens);
    } else if ((command == "KICK" || command == "INVITE" || command == "TOPIC" || command == "MODE") 
                && client.GetAuthenticated() == true && client.GetClientNick() != ""){
        if (!tokens.empty()) {
            handleOperatorCommand(client, server, command, tokens);
        } else {
            std::string Msg =  "ERROR: Command requires a channel\n" ;
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        }
    } else if (command == "LIST" && client.GetAuthenticated() == true) {
        ListChannels(client, server); // Corrected to pass the server instance
    } else if (command != "CAP" && command != "WHO")  {
        std::string msg =  "ERROR: Unknown command: " + command + "\n" ;
        send(client.GetClientSocket(), msg.c_str(), msg.size(), 0);
    } else if (command == "DCC" && tokens[0] == "SEND") {
        processDCCSend(client, tokens);
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
    // ERR_ALREADYREGISTERED (462)
    if (client.GetAuthenticated()) {
        Utiles::sendNumericReply(client, 462, ":You are already registered");
        return;
    }
    else if (args.empty()) {
        // ERR_NEEDMOREPARAMS (461)
        Utiles::sendNumericReply(client, 461, "PASS :Not enough parameters");
        server.RemoveClient(client.GetClientSocket());
        return;
    } else if (server.GetPassword() != args[0]) {
        // ERR_PASSWDMISMATCH (464)
        Utiles::sendNumericReply(client, 464, ":Password incorrect");
        server.RemoveClient(client.GetClientSocket());
        return;
    }

    std::string password = args[0];
    if (server.GetPassword() == password) {
        client.SetAuthenticated(true);
    }
}

void CommandHandler::processNick(Client &client, Server &server, const std::vector<std::string> &args)
{
    std::string Msg;
    std::string serverName = "ft_irc";
    std::string nick = args.size() > 0 ? args[0] : "";

    // ERR_NONICKNAMEGIVEN (431)
    if (nick.empty()) {
        Utiles::sendNumericReply(client, 431, ":No nickname given"); 
        return;
    }

    // ERR_NICKNAMEINUSE (433) 
    if (client.GetClientNick() == nick) {
        Utiles::sendNumericReply(client, 433, nick + " :Nickname is already in use");
        return;
    }

    // ERR_NICKNAMEINUSE (433)
    for (size_t i = 0; i < server.GetClients().size(); i++) {
        if (server.GetClients()[i].GetClientNick() == nick && server.GetClients()[i].GetClientSocket() != client.GetClientSocket()) {
            Utiles::sendNumericReply(client, 433, nick + " :Nickname is already in use"); 
            return;
        }
    }

    // ERR_ERRONEUSNICKNAME (432)
    if (!client.isValidNick(nick)) {
        Utiles::sendNumericReply(client, 432, nick + " :Erroneous nickname");
        return;
    }

    // Establecer el nick del cliente
    client.SetClientNick(nick);
    client.SetNickSet(true);
    std::cout << Server::getCurrentTime() 
              << GREEN << "[+] Client <" << client.GetClientSocket() << "> set nickname to " 
              << MAGENTA << nick << RESET << std::endl;

    // Enviar mensajes de bienvenida y MOTD
    if (client.getUser()) {
        Utiles::sendWelcomeMessage(client);
    } else {
        // Mensaje para completar el comando USER 
        Msg = "Your Nickname has been set. Use USER command to continue\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    }
}

void CommandHandler::processUser(Client &client, Server &/*server*/, const std::vector<std::string> &args)
{
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
    std::string Msg;
    // Comprobar que USER y NICK fueron creados
    if (client.getUser() == false || client.getNick() == false) {
        Msg = "ERROR: You must set NICK and USER before joining a channel\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // Verificar se se proporcionó el nombre del canal
    if (args.size() < 1){
        Msg = "ERROR: JOIN requires a channel: JOIN <#channel>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // Verficar que el nombre del canal comience con #
    std::string channel = args[0];
    //printf("Channel: %s\n", channel.c_str());
    if (channel[0] != '#') {
        Msg = "ERROR: Channel name must start with '#': JOIN <#channel>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // LLamar al servidor para añadir el cliente al canal
    server.JoinChannel(client, channel, args);
    
    // Enviar la lista de usuarios en el canal
    Channel* channelObj = server.GetThisChannel(channel);
    std::vector<int> users = channelObj->GetUsers();
    std::string userList = "= " + channel + " :";
    for (size_t i = 0; i < users.size(); i++) {
        Client& user = server.GetClientBySocket(users[i]);
        userList += user.GetClientNick() + " ";
    }
    userList += "\n";
}

void CommandHandler::processPrivmsg(Client &client, Server &server, const std::vector<std::string> &args)
{
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

    if (!message.empty() && message[0] == ':') {
        message = message.substr(1);
    } else {
        std::string Msg = "ERROR: Invalid message format. Use PRIVMSG <nick or #channel> :<message>\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // Verificar si el cliente está dentro del canal #42bot
    if (destinatary == "#42bot") {
        sendMessageToBot(client, server, args);
    }


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

void CommandHandler::sendMessageToBot(Client &client, Server &server, const std::vector<std::string> &args)
{
   // Concatenar el mensaje completo
    std::string message = args[1];
    for (size_t i = 2; i < args.size(); ++i) {
        message += " " + args[i];
    }

    // Eliminar el prefijo ':' si está presente
    if (!message.empty() && message[0] == ':') {
        message.erase(0, 1);
    }

    // Obtener el canal
    Channel* channel = server.GetThisChannel("#42bot");
    if (channel == NULL) {
        std::string errMsg = "ERROR: No such channel\n";
        send(client.GetClientSocket(), errMsg.c_str(), errMsg.size(), 0);
        return;
    }

    // Obtener el socket del bot
    Client* botClient = server.GetUserByNick("42bot");
    if (botClient == NULL) {
        std::string errMsg = "ERROR: Bot not found\n";
        send(client.GetClientSocket(), errMsg.c_str(), errMsg.size(), 0);
        return;
    }

    // Comprobar si el mensaje es el comando !mod
    if (message == "!mod") {
        // Enviar el mensaje del día a todos los usuarios en el canal #42bot
        std::string motd = ":" + botClient->GetClientNick() + "!" + botClient->GetUsername() + "@" + botClient->GetHostname() + " PRIVMSG #42bot :" + IRCBot::sendMessagesOfDay() + "\n";
        server.broadcastToChannel(*channel, motd, botClient->GetClientSocket());
    } else {
        // Enviar mensaje de bienvenida a todos los usuarios en el canal #42bot
        std::string welcomeMsg = ":" + botClient->GetClientNick() + "!" + botClient->GetUsername() + "@" + botClient->GetHostname() + " PRIVMSG #42bot :Welcome to the #42bot channel! Type !mod to receive the message of the day.\n";
        server.broadcastToChannel(*channel, welcomeMsg, botClient->GetClientSocket());
    }
}

void CommandHandler::processKick(Client &client, Server &server, const std::vector<std::string> &args)
{
    // Expulsar al usuario del canal
    if (args.size() < 2) {
    // ERR_NEEDMOREPARAMS (461)
        Utiles::sendNumericReply(client, 461, "KICK :Not enough parameters");
        return;
    }

    std::string channelName = args[0];
    std::string nickToKick = args[1];
    std::string reason = (args.size() > 2) ? args[2] : "No reason given";

    Channel* channel = server.GetThisChannel(channelName);

    if (nickToKick[0] == ':') {
        nickToKick = nickToKick.substr(1);
    }
    Client *targetClient = server.GetUserByNick(nickToKick);
    if (targetClient == nullptr) {
        // ERR_NOSUCHNICK (401)
        std::string errorMsg = ":ft_irc 401 " + client.GetClientNick() + " " + nickToKick + " :No such nick\n";
        send(client.GetClientSocket(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }

    if (!channel->IsUserInChannel(targetClient->GetClientSocket())) {
        // ERR_USERNOTINCHANNEL (441)
        std::string errorMsg = ":ft_irc 441 " + client.GetClientNick() + " " + nickToKick + " " + channelName + " :They aren't on that channel\n";
        send(client.GetClientSocket(), errorMsg.c_str(), errorMsg.size(), 0);
        return;
    }

    // Notificar al usuario expulsado
    std::string kickNotification = ":" + client.GetClientNick() + "!" + client.GetUsername() + "@" + client.GetHostname() + " KICK " + channelName + " " + nickToKick + " :" + reason + "\n";
    send(targetClient->GetClientSocket(), kickNotification.c_str(), kickNotification.size(), 0);

    // Actualizar la lista de usuarios del canal
    std::vector<int> channelUsers = channel->GetUsers();
    for (size_t i = 0; i < channelUsers.size(); ++i) {
        send(channelUsers[i], kickNotification.c_str(), kickNotification.size(), 0);
    }

    channel->RemoveUser(targetClient->GetClientSocket());
    server.updateUserList(*channel);
}

void CommandHandler::processInvite(Client &client, Server &server, const std::vector<std::string> &args)
{
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
    channelObj->inviteUser(destSocket);
    std::string inviteMsg = ":" + client.GetClientNick() + " INVITE " + nick + " :" + channel + "\r\n";    send(destSocket, inviteMsg.c_str(), inviteMsg.size(), 0);
    Utiles::sendNumericReply(client, 341, nick + " " + channel);
}

void CommandHandler::processTopic(Client &client, Server &server, const std::vector<std::string> &args)
{
    // Segun protocolo
    if (args.size() < 1) {
        // Enviar error si no se proporciona el nombre del canal
        Utiles::sendNumericReply(client, 461, "TOPIC :Not enough parameters");
        return;
    }

    std::string channelName = args[0];
    Channel* channel = server.GetThisChannel(channelName);

    if (args.size() == 1) {
        // Si solo se proporciona el nombre del canal, devolver el tema actual
        std::string currentTopic = channel->getTopic();
        if (currentTopic.empty()) {
            Utiles::sendNumericReply(client, 331, channelName + " :No topic is set");
        } else {
            Utiles::sendNumericReply(client, 332, channelName + " " + currentTopic);
        }
    } else {
        // Verificar si el cliente es miembro del canal
        if (!channel->IsUserInChannel(client.GetClientSocket())) {
            Utiles::sendNumericReply(client, 442, channelName + " :You're not on that channel");
            return;
        }
        // Verificar si el canal tiene el modo +t activado
        bool isOperator = channel->IsOperator(client.GetClientSocket());

        if (channel->isTopicblock() && !isOperator) {
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
        
        // Eliminar los dos puntos (:) del principio si estan presentes
        if (!newTopic.empty() && newTopic[0] == ':') {
            newTopic = newTopic.substr(1);
        } else {
            std::string Msg;
            Msg = "ERROR: Invalid topic format. Use TOPIC <#channel> :<topic>\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }

        channel->setTopic(newTopic);
        std::string msg = ":" + client.GetClientNick() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
        channel->BroadcastMessage(msg, server);
    }
}

void CommandHandler::processMode(Client &client, Server &server, const std::vector<std::string> &args)
{
   std::string Msg;
    //RPL_UMODEIS (324) - Modestring not given
    if (args.size() < 2) {
        return;
    }

    std::string channel = args[0];
    Channel* channelObj = server.GetThisChannel(channel);
    //ERR_NOSUCHCHANNEL (403) - Channel does not exist
    if (!channelObj) {
        Utiles::sendNumericReply(client, 403, "Channel does not exist");
        return;
    }

    size_t i = 1;
    bool adding = true;
    while (i < args.size()) {
        std::string mode = args[i];
        if (mode[0] == '+' || mode[0] == '-') {
            adding = (mode[0] == '+');
            for (size_t j = 1; j < mode.size(); ++j) {
                char flag = mode[j];
                switch (flag) {
                    case 'i':
                        channelObj->setInviteOnly(adding);
                        Msg = adding ?  "Invite only mode has been set\n"  :  "Invite only mode has been removed\n" ;
                        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
                        break;
                    case 't':
                        channelObj->setTopicblock(adding);
                        Msg = adding ?  "Topic block mode has been set\n"  :  "Topic block mode has been removed\n" ;
                        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
                        break;
                    case 'o':
                        if (i + 1 < args.size()) {
                            std::string nick = args[++i];
                            int destSocket = server.GetSocketByNick(nick);
                            if (channelObj->UserExists(destSocket)) {
                                if (adding) {
                                    channelObj->addOperator(destSocket);
                                    server.updateUserList(*channelObj);
                                    Msg = "User has been set as operator\n";
                                } else {
                                    channelObj->removeOperator(destSocket);
                                    server.updateUserList(*channelObj);
                                    Msg =  "User has been removed as operator\n" ;
                                }
                                send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
                                Msg = adding ?  "You are now an operator of channel " + channel + "\n"  :  "You are no longer an operator of channel " + channel + "\n" ;
                                send(destSocket, Msg.c_str(), Msg.size(), 0);
                            } 
                        } 
                        break;
                    case 'k':
                        if (adding) {
                            if (i + 1 < args.size()) {
                                std::string password = args[++i];
                                channelObj->setPassword(password);
                                channelObj->setModeKEnabled(true);
                                Msg =  "Password has been set\n";
                            }
                        } else {
                            channelObj->setPassword("");
                            channelObj->setModeKEnabled(false);
                            Msg =  "Password has been removed\n";
                        }
                        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
                        break;
                    case 'l':
                        if (adding) {
                            if (i + 1 < args.size()) {
                                unsigned int limit;
                                std::stringstream ss(args[++i]);
                                if ((ss >> limit) && ss.eof()) {
                                    channelObj->setLimitUsers(limit);
                                    channelObj->setLimitUsersEnabled(true);
                                    Msg =  "User limit has been set\n";
                                }
                            } 
                        } else {
                            channelObj->setLimitUsers(0);
                            channelObj->setLimitUsersEnabled(false);
                            Msg =  "User limit has been removed\n";
                        }
                        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
                        break;
                }
            }
        } 
        ++i;
    }
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
        std::string Msg = "ERROR: Command requires a valid channel\n";
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
            if (command == "MODE")
                return false;
            std::string Msg = "ERROR: You are not an operator in this channel\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return false;
        }
    }
    std::cout  << Server::getCurrentTime() 
                << GREEN << "[+] Client <" << client.GetClientSocket() << "> executed operator command: " << MAGENTA << command << RESET << std::endl;
    return true;
}

// ********************************************
// *            BONUS PART: DCC SEND          *
// ********************************************

void CommandHandler::processDCCSend(Client &client, const std::vector<std::string> &args)
{
    if (args.size() < 4) {
        Utiles::sendNumericReply(client, 461, "DCC SEND :Not enough parameters");
        return;
    }

    std::string filename = args[1];
    std::string ip = args[2];

    // Convertir cadena a entero
    std::istringstream iss(args[3]);
    int port;
    if (!(iss >> port)) {
        Utiles::sendNumericReply(client, 461, "DCC SEND :Invalid port number");
        return;
    }

    // Convertir entero a cadena
    std::ostringstream oss;
    oss << port;
    std::string portStr = oss.str();

    // Enviar solicitud de transferencia de archivos al cliente destinatario
    std::string dccRequest = "DCC SEND " + filename + " " + ip + " " + portStr;
    send(client.GetClientSocket(), dccRequest.c_str(), dccRequest.size(), 0);

    std::cout << Server::getCurrentTime() 
              << GREEN << "[+] Client <" << client.GetClientSocket() << "> sent DCC SEND request for file " 
              << MAGENTA << filename << RESET << std::endl;
}

void CommandHandler::ListChannels(Client &client, Server &server) 
{
    std::string Msg;
    std::string serverName = "myserver"; // Nombre del servidor, cámbialo según sea necesario

    // Enviar RPL_LISTSTART (321)
    Msg = ":" + serverName + " 321 " + client.GetClientNick() + " Channel :Users Name\n";
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);

    // Obtener la lista de canales
    std::vector<Channel> channels = server.GetAllChannels();
    for (size_t i = 0; i < channels.size(); i++) {
        // Enviar RPL_LIST (322) para cada canal
        Msg = ":" + serverName + " 322 " + client.GetClientNick() + " " + channels[i].GetName() + " " + Utiles::toString(channels[i].GetUsers().size()) + " :\n";
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);

        // Enviar el topic del canal, si existe
        if (channels[i].HasTopic()) {
            std::string topic = channels[i].getTopic();
            Msg = ":" + serverName + " 332 " + client.GetClientNick() + " " + channels[i].GetName() + " :" + topic + "\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        }
    }

    // Enviar RPL_LISTEND (323)
    Msg = ":" + serverName + " 323 " + client.GetClientNick() + " :End of /LIST\n";
    send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
}