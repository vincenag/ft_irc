#include "Library.hpp"

Server::Server() 
{
    this->serverSocket = -1;
}

Server::~Server(){}

Server::Server(Server const &src){*this = src;}

Server &Server::operator=(Server const &src) // modificar esto luego
{
    if (this != &src)
    {
        this->password = src.password;
        this->port = src.port;
        this->serverSocket = src.serverSocket;
        this->address = src.address;
        this->fds = src.fds;
        this->new_poll = src.new_poll;
        this->clients = src.clients;
        this->channels = src.channels;
        //this->clientNicknames = src.clientNicknames;
    }
    return *this;
}

bool Server::Signal = false;

/**
    * @brief Imprime un banner en la consola para cada cliente que se conecta.
 */
void Server::printIRCChatBanner(int clientSocket)
{
    std::string banner =     "  8 8888 8 888888888o.       ,o888888o.        \n";
    banner +=                "  8 8888 8 8888    `88.     8888     `88.      \n";
    banner +=                "  8 8888 8 8888     `88  ,8 8888       `8.     \n";
    banner +=                "  8 8888 8 8888     ,88  88 8888               \n";
    banner +=                "  8 8888 8 8888.   ,88'  88 8888               \n";
    banner +=                "  8 8888 8 888888888P'   88 8888               \n";
    banner +=                "  8 8888 8 8888`8b       88 8888               \n";
    banner +=                "  8 8888 8 8888 `8b.     `8 8888       .8'     \n";
    banner +=                "  8 8888 8 8888   `8b.      8888     ,88'      \n";
    banner +=                "  8 8888 8 8888     `88.     `8888888P'        \n";


    banner += "\n\n";
    banner +=  "** Welcome to the IRC Chat **\n\n" ;
    banner +=  "Please enter the password to continue: "  "PASS <password>\n";

    send(clientSocket, banner.c_str(), banner.size(), 0);
}

/**
 * @brief Manejador de señales.
 *
 * Esta función estática se llama cuando se recibe una señal.
 * Establece la variable estática `Signal` en `true` para indicar que se ha recibido una señal.
 *
 * @param signal El número de la señal recibida.
 */
void Server::signalHandler(int signal)
{
    (void)signal;
    std::cout << Server::getCurrentTime() << GREEN << "[+] Signal received" << RESET << std::endl;
    Signal = true;
}

/**
 * @brief Inicializa el servidor y maneja las conexiones entrantes y los datos de los clientes.
 *
 * Esta función configura el servidor con el puerto y la contraseña proporcionados,
 * inicializa el socket del servidor y entra en un bucle que utiliza `poll` para 
 * manejar conexiones y datos entrantes de los clientes. Si el `poll` detecta que 
 * hay datos para leer en el socket del servidor, acepta una nueva conexión de cliente.
 * Si hay datos para leer en los sockets de los clientes existentes, llama a la 
 * función para procesar esos datos.
 *
 * @param port El número de puerto en el que el servidor escuchará las conexiones entrantes.
 * @param password La contraseña que los clientes deben proporcionar para conectarse al servidor.
 *
 * @throw std::runtime_error Si ocurre un error en la inicialización del socket, en `poll`, 
 *        al aceptar una nueva conexión de cliente o al procesar los datos del cliente.
 */
void Server::serverInit(int port, std::string password)
{
    this->port = port;
    this->password = password;

    std::cout   << Server::getCurrentTime()
                << GREEN << "[+] Server initialized" << RESET << std::endl;

    this->socketInit();

    while (Server::Signal == false)
    {
        int ret = poll(&fds[0], fds.size(), -1); // -1: Esperar indefinidamente hasta que se produzca un evento
        if (ret < 0){
            if (errno == EINTR) // EINTR: Señal recibida
                continue;
            throw std::runtime_error("poll() failed");
        }
        if (fds[0].revents & POLLIN) // POLLIN: Hay datos para leer
        {
            this->acceptClient();
        }
        for (size_t i = 1; i < fds.size(); i++)
        {
            if (fds[i].revents & POLLIN)
            {
                this->getClientdata(fds[i].fd);
            }
        }
    }

    this->shutdownServer();
}

/**
 * @brief Apaga el servidor.
 * Esta función cierra el socket del servidor y los sockets de los clientes.
 */
void Server::shutdownServer()
{
    std::cout   << Server::getCurrentTime()
                << RED << "[-] Shutting down server" << RESET << std::endl;

    close(this->serverSocket);
    for (size_t i = 1; i < fds.size(); i++)
    {
        close(fds[i].fd);
    }
}


/**
 * @brief Inicializa el socket del servidor y lo configura para escuchar conexiones entrantes.
 *
 * Esta función crea el socket, establece sus opciones, lo configura para el modo no bloqueante,
 * lo liga a la dirección y puerto especificados, y comienza a escuchar conexiones entrantes.
 * También agrega el socket del servidor a la lista de descriptores de archivo para monitorear eventos de entrada.
 * Finalmente, conecta un bot al servidor.
 *
 * @throw std::runtime_error Si ocurre un error durante la creación, configuración, enlace, o escucha del socket.
 */
void Server::socketInit() 
{
    int opt = 1;

    this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->serverSocket == -1)
        throw std::runtime_error("Error creating socket");

    this->address.sin_family = AF_INET;         // Address family = Internet
    this->address.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY permite que el socket escuche en todas las interfaces de red
    this->address.sin_port = htons(this->port); // htons() convierte el número de puerto al formato de red

    if (setsockopt(this->serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) // Permite reutilizar el socket
        throw std::runtime_error("Error setting socket options");

    if (fcntl(this->serverSocket, F_SETFL, O_NONBLOCK) < 0) // Establece el socket en modo no bloqueante
        throw std::runtime_error("Error setting socket to non-blocking mode");

    if (bind(this->serverSocket, (struct sockaddr *)&this->address, sizeof(this->address)) < 0) // Liga el socket a la dirección y puerto
        throw std::runtime_error("Error binding socket");

    if (listen(this->serverSocket, SOMAXCONN) < 0)  // Escucha las conexiones entrantes
        throw std::runtime_error("Error listening socket");

    new_poll.fd = this->serverSocket; 
    new_poll.events = POLLIN; // POLLIN: Hay datos para leer
    fds.push_back(new_poll);

    std::cout   << Server::getCurrentTime() 
                << GREEN << "[+] Server listening on port " 
                << MAGENTA << this->port << RESET << std::endl;

    // ! Conectar el Bot al server
    std::string serverBot = "127.0.0.1";
    std::string channel = "#42bot";
    std::string nick = "42bot";
    std::string user = "42bot 0 * :42bot";
    IRCBot bot(serverBot, port, channel, nick, user, password);
}


/**
 * @brief Acepta una nueva conexión de cliente y configura su socket.
 *
 * Esta función acepta una nueva conexión de cliente, establece el socket del cliente en modo no bloqueante,
 * agrega el socket a la lista de descriptores de archivo para monitorear eventos de entrada, crea una nueva
 * instancia de `Client`, y lo inicializa con la información del socket y la dirección IP del cliente.
 * Finalmente, imprime un mensaje en la consola indicando la conexión del nuevo cliente y muestra el banner
 * de chat IRC al cliente.
 *
 * @throw std::runtime_error Si ocurre un error al aceptar la conexión del cliente o al configurar el socket en modo no bloqueante.
 */
void Server::acceptClient()
{
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = accept(this->serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength); // Aceptar la conexión del cliente
    if (clientSocket < 0)
        throw std::runtime_error("Error accepting client connection");
    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0) // Establecer el socket en modo no bloqueante
        throw std::runtime_error("Error setting socket to non-blocking mode");

    new_poll.fd = clientSocket;
    new_poll.events = POLLIN;
    fds.push_back(new_poll);
  
    Client newClient;
    
    newClient.SetClientSocket(clientSocket);
    newClient.SetClientIpAddr(inet_ntoa(clientAddress.sin_addr));
    newClient.SetAuthenticated(false);

    std::cout   << Server::getCurrentTime() 
                << GREEN << "[+] New client connected. IP: " 
                << MAGENTA << newClient.GetClientIpAddr() << RESET << std::endl;

    clients.push_back(newClient);

    printIRCChatBanner(clientSocket);
}

/**
 * @brief Procesa los datos recibidos de un cliente.
 *
 * Esta función recibe los datos del cliente, los imprime en la consola y realiza cualquier procesamiento adicional necesario.
 * Si el cliente se desconecta, cierra el socket del cliente y elimina la información del cliente de la lista de clientes.
 *
 * @param clientSocket El descriptor de archivo del socket del cliente.
 */
void Server::getClientdata(int clientSocket)
{
    char buff[1024]; // Buffer para los datos recibidos
    memset(buff, 0, sizeof(buff)); // Limpiar el buffer

    ssize_t bytes = recv(clientSocket, buff, sizeof(buff) - 1, 0); // Recibir los datos

    if (bytes <= 0) { // Comprobar si el cliente se desconectó
        std::cout   << Server::getCurrentTime()
                    << RED << "[-] Client <" << clientSocket << "> Disconnected" << RESET << std::endl;
        
        RemoveClient(clientSocket);

    } else { // Procesar los datos recibidos
        buff[bytes] = '\0';
        // Convertir el buffer en string
        std::string data(buff);
        // Buscar el objeto Client correspondiente al clientSocket
        Client* client = nullptr;
        for (size_t i = 0; i < this->clients.size(); ++i) {
            if (this->clients[i].GetClientSocket() == clientSocket) {
                client = &this->clients[i];
                break;
            }
        }
        if (client != nullptr) {
            // Agregar los datos recibidos al buffer persistente del cliente
            client->getBuffer() += data;

            // Procesar los comandos completos en el buffer
            size_t pos;
            while ((pos = client->getBuffer().find('\n')) != std::string::npos) {
                

                std::string command = client->getBuffer().substr(0, pos);
                client->getBuffer().erase(0, pos + 1);


                // Crear una instancia de CommandHandler y manejar el comando
                CommandHandler cmdHandler;
                cmdHandler.handleCommand(command, *this, *client);
            }
        }
    }
}

/**
 * @brief Elimina un cliente de la lista de clientes.
 * Esta función elimina un cliente de la lista de clientes y elimina el descriptor de archivo del cliente de la lista de descriptores de archivo.
 * También elimina al cliente de todos los canales a los que pertenece.
 * @param clientSocket El descriptor de archivo del socket del cliente. 
 */
void Server::RemoveClient(int clientSocket)
{
    for (size_t i = 0; i < this->clients.size(); i++)
    {
        if (this->clients[i].GetClientSocket() == clientSocket)
        {
            this->clients.erase(this->clients.begin() + i);
            break;
        }
    }
    for (size_t i = 0; i < this->fds.size(); i++)
    {
        if (this->fds[i].fd == clientSocket)
        {
            this->fds.erase(this->fds.begin() + i);
            break;
        }
    }
    close(clientSocket);

    // Eliminar al cliente de todos los canales
    for (size_t i = 0; i < this->channels.size(); i++)
    {
        this->channels[i].RemoveUser(clientSocket);
        this->updateUserList(this->channels[i]);
    }

}

/**
 * @brief Agrega un cliente a un canal.
 * Esta función agrega un cliente a un canal existente o crea un nuevo canal y agrega al cliente.
 * @param client El cliente que se unirá al canal.
 * @param channelName El nombre del canal al que se unirá el cliente.
 * @param args Los argumentos adicionales proporcionados por el cliente.
 * @throw std::runtime_error Si el cliente ya está en el canal.
 * @throw std::runtime_error Si el canal está lleno.
 * @throw std::runtime_error Si el cliente no está invitado al canal.
 * @throw std::runtime_error Si la contraseña del canal es incorrecta.
 * 
 */
void Server::JoinChannel(Client &client, std::string channelName, const std::vector<std::string> &args)
{
   bool channelFound = ChannelExists(channelName);

    if (channelFound) {

        // Comprobar si el canal requiere contraseña
        Channel* channel = GetThisChannel(channelName);
        if (channel->isModeKEnabled()) {
            if (args.size() < 1 || args[1] != channel->getPassword()) {
                std::string Msg = "ERROR: Invalid password\n";
                send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
                return;
            }
        }

        // Comprobar si el canal es solo por invitación y si el cliente está invitado
        if (channel->isInviteOnly() && !channel->IsInvited(client.GetClientSocket())) {
            std::string Msg = "ERROR: You are not invited to this channel\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }

        // Comprobar si el canal tiene un límite de usuarios y si se ha alcanzado
        if (channel->isLimitUsersEnabled() && channel->GetUsers().size() >= channel->getLimitUsers()) {
            std::string Msg = "ERROR: Channel is full\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }

        // Comprobar si el cliente ya está en el canal
        if (channel->UserExists(client.GetClientSocket())) {
            std::string Msg = "ERROR: You are already in this channel\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }

        // Añadir al cliente al canal
        channel->AddUser(client.GetClientSocket());
        client.SetChannel(channelName); 

        // Enviar mensajes de bienvenida al canal
        sendJoinMessages(client, *channel);

        std::cout   << Server::getCurrentTime() 
                    << GREEN << "[+] Client <" << client.GetClientSocket() << "> joined channel " 
                    << MAGENTA << channelName << RESET << std::endl;
        return;
    }

    // Si el canal no existe, se crea uno nuevo y se añade al cliente como operador
    if (!channelFound) {
        Channel newChannel(channelName);
        newChannel.AddUser(client.GetClientSocket());
        newChannel.addOperator(client.GetClientSocket());
        this->channels.push_back(newChannel);
        client.SetChannel(channelName);

        // Enviar mensajes de bienvenida al canal
        sendJoinMessages(client, newChannel);

        std::cout   << Server::getCurrentTime() 
                    << GREEN << "[+] Client <" << client.GetClientSocket() << "> has created a new channel: " 
                    << MAGENTA << channelName << RESET << std::endl;
        std::cout   << Server::getCurrentTime()
                    << BLUE << "Assigned as Admin of channel: " 
                    << MAGENTA << channelName << RESET 
                    << BLUE << " to Client: "
                    << MAGENTA << client.GetClientSocket() << RESET << std::endl;
    }
}

/**
 * @brief Envía los mensajes de JOIN apropiados cuando un cliente se une a un canal.
 *
 * Esta función envía el mensaje JOIN a todos los usuarios en el canal, incluido el usuario que se está uniendo.
 * También envía la respuesta numérica RPL_TOPIC (332) si el canal tiene un tema establecido, o RPL_NOTOPIC (331)
 * si no hay un tema establecido. Finalmente, actualiza la lista de usuarios del canal.
 *
 * @param client El cliente que se está uniendo al canal.
 * @param channel El canal al que el cliente se está uniendo.
 */
void Server::sendJoinMessages(Client &client, Channel &channel)
{
    std::string nick = client.GetClientNick();
    std::string channelName = channel.GetName();
    std::string serverName = "ft_irc";

    // Enviar el mensaje JOIN a todos los usuarios del canal, incluido el que se une
    std::string joinMsg = ":" + nick + "!" + client.GetUsername() + "@" + client.GetHostname() + " JOIN :" + channelName + "\n";
    broadcastToChannel(channel, joinMsg, -1);

    // Enviar el mensaje RPL_TOPIC (332) si el canal tiene un tema establecido
    std::string topic = channel.getTopic();
    std::string topicMsg;

    if (!topic.empty()) {
        topicMsg = ":" + serverName + " 332 " + nick + " " + channelName + " :" + topic + "\n";
    } else {
        topicMsg = ":" + serverName + " 331 " + nick + " " + channelName + " :No topic is set\n";
    }
    send(client.GetClientSocket(), topicMsg.c_str(), topicMsg.size(), 0);

    // Construir y enviar el mensaje RPL_NAMREPLY (353)
    Server::updateUserList(channel);
}

/**
 * @brief Envía un mensaje a todos los usuarios en un canal, excepto al usuario especificado.
 *
 * Esta función recorre todos los usuarios en el canal y envía el mensaje proporcionado a cada usuario.
 * Si el `clientSocket` proporcionado es diferente de -1, ese usuario será excluido de la transmisión.
 *
 * @param channel El canal cuyos usuarios recibirán el mensaje.
 * @param message El mensaje que se enviará a los usuarios del canal.
 * @param clientSocket El socket del cliente que no debería recibir el mensaje. Si es -1, todos los usuarios reciben el mensaje.
 */
void Server::broadcastToChannel(Channel &channel, const std::string &message, int clientSocket)
{
    std::vector<int> users = channel.GetUsers(); // Asume que GetUsers devuelve un std::vector<int>
    for (std::vector<int>::iterator it = users.begin(); it != users.end(); ++it) {
        int userSocket = *it;
        if (userSocket != clientSocket) {
            send(userSocket, message.c_str(), message.size(), 0);
        }
    }
}

/**
 * @brief Actualiza y envía la lista de usuarios de un canal a todos los usuarios en ese canal.
 *
 * Esta función construye y envía los mensajes RPL_NAMREPLY (353) y RPL_ENDOFNAMES (366) a cada usuario en el canal,
 * proporcionando una lista de los usuarios actuales en el canal.
 *
 * @param channel El canal del cual se obtendrán y enviarán las listas de usuarios.
 */
void Server::updateUserList(Channel &channel)
{
    std::vector<int> users = channel.GetUsers();
    for (std::vector<int>::iterator it = users.begin(); it != users.end(); ++it) {
        Client* user = GetThisClient(*it);
        if (user) {
            std::string nick = user->GetClientNick();
            std::string channelName = channel.GetName();
            std::string serverName = "ft_irc"; 

            std::string namesPrefix = ":" + serverName + " 353 " + nick + " = " + channelName + " :";
            std::string userList;
            for (std::vector<int>::iterator it2 = users.begin(); it2 != users.end(); ++it2) {
                Client* user2 = GetThisClient(*it2);
                if (user2) {
                    std::string userNick = user2->GetClientNick();
                    if (channel.IsOperator(*it2)) {
                        userNick = "@" + userNick;
                    }
                    userList += userNick + " ";
                }
            }
            userList += "\n";
            std::string namesMsg = namesPrefix + userList;
            send(user->GetClientSocket(), namesMsg.c_str(), namesMsg.size(), 0);

            std::string endNamesMsg = ":" + serverName + " 366 " + nick + " " + channelName + " :End of /NAMES list\n";
            send(user->GetClientSocket(), endNamesMsg.c_str(), endNamesMsg.size(), 0);
        }
    }
}

Client* Server::GetThisClient(int clientSocket)
{
    for (size_t i = 0; i < this->clients.size(); i++)
    {
        if (this->clients[i].GetClientSocket() == clientSocket)
            return &this->clients[i];
    }
    return nullptr;
}


std::string Server::GetPassword()
{
    return this->password;
}

std::vector<Channel> Server::GetChannels()
{
    return this->channels;
}


std::vector<Client> Server::GetClients()
{
    return this->clients;
}

int Server::GetSocketByNick(const std::string& nick) const
{
    for (size_t i = 0; i < this->clients.size(); i++)
    {
        if (this->clients[i].GetClientNick() == nick)
            return this->clients[i].GetClientSocket();
    }
    return -1;
}

Client& Server::GetClientBySocket(int socketId)
{
    for (size_t i = 0; i < this->clients.size(); i++)
    {
        if (this->clients[i].GetClientSocket() == socketId)
            return this->clients[i];
    }
    throw std::runtime_error("Client not found");
}

bool Server::ChannelExists(std::string channelName)
{
    for (size_t i = 0; i < this->channels.size(); i++)
    {
        if (this->channels[i].GetName() == channelName)
            return true;
    }
    return false;
}

Channel* Server::GetThisChannel(std::string channelName)
{
    for (size_t i = 0; i < this->channels.size(); i++)
    {
        if (this->channels[i].GetName() == channelName)
            return &this->channels[i];
    }
    return nullptr;
}

Client* Server::GetUserByNick(const std::string& nick)
{
    if (isUser(nick)) { 
        int socketId = GetSocketByNick(nick); 
        if (socketId != -1) { 
            return GetThisClient(socketId); 
        }
    }
    return nullptr; 
}

bool Server::isUser(const std::string &nick) const
{
    for (size_t i = 0; i < this->clients.size(); i++)
    {
        if (this->clients[i].GetClientNick() == nick)
            return true;
    }
    return false;
}

std::vector<Channel> Server::GetAllChannels() const
{
    return this->channels;
}

int Server::GetSocket(const std::string &nick) const
{
    for (size_t i = 0; i < this->clients.size(); i++)
    {
        if (this->clients[i].GetClientNick() == nick)
            return this->clients[i].GetClientSocket();
    }
    return -1;
}

/**
    * @brief Función que obtiene fecha y hora actuales
 */
std::string Server::getCurrentTime()
{
    std::time_t now = std::time(0);
    std::tm *nowLocal = std::localtime(&now);

    std::stringstream ss;
    ss << CYAN << std::setfill('0') << std::setw(2) << nowLocal->tm_mday << '/'
       << std::setfill('0') << std::setw(2) << (nowLocal->tm_mon + 1) << "/"
       << (nowLocal->tm_year + 1900) << " " << RESET
       << YELLOW << std::setfill('0') << std::setw(2) << nowLocal->tm_hour << ":" 
       << std::setfill('0') << std::setw(2) << nowLocal->tm_min << ":" 
       << std::setfill('0') << std::setw(2) << nowLocal->tm_sec << " " << RESET;

    return ss.str();
}

/**
 * @brief Maneja la reconexión de un cliente, procesando cualquier comando en cola.
 *
 * Esta función marca al cliente como reconectado, procesa cualquier comando pendiente en su cola de comandos,
 * y limpia su buffer de comandos.
 *
 * @param clientSocket El socket del cliente que se está reconectando.
 */
void Server::handleClientReconnection(int clientSocket) {
    for (std::vector<Client>::iterator it = this->clients.begin(); it != this->clients.end(); ++it) {
        if (it->GetClientSocket() == clientSocket) {
            it->SetDisconnected(false);

            while (!it->getCommandQueue().empty()) {
                std::string command = it->getCommandQueue().front();
                it->getCommandQueue().pop();

                CommandHandler cmdHandler;
                cmdHandler.handleCommand(command, *this, *it);
            }

            it->clearBuffer();
            break;
        }
    }
}

