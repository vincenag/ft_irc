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
 * @brief Inicializa el socket del servidor.
 *
 * Esta función crea un socket para el servidor, configura las opciones del socket,
 * lo establece en modo no bloqueante, lo liga a una dirección y puerto específicos,
 * y lo pone en modo de escucha para aceptar conexiones entrantes. 
 * También agrega el socket del servidor a la estructura `poll` para monitorear eventos.
 *
 * @throw std::runtime_error Si ocurre un error al crear el socket, configurar opciones,
 *        establecer el modo no bloqueante, ligar el socket o ponerlo en modo de escucha.
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
}


/**
 * @brief Acepta una nueva conexión de cliente.
 *
 * Esta función acepta una conexión entrante en el socket del servidor, configura el socket del cliente en modo no bloqueante,
 * crea una nueva instancia de la clase Client y la agrega a la lista de clientes.
 *
 * @throw std::runtime_error Si ocurre un error al aceptar la conexión del cliente o al establecer el socket en modo no bloqueante.
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
}

void Server::JoinChannel(Client &client, std::string channelName, const std::vector<std::string> &args)
{
    /* // Comprobar si el canal ya existe
    bool channelFound = ChannelExists(channelName);

    // Si el canal existe, se añade al cliente
    if (channelFound){
        Channel* channel = GetThisChannel(channelName);
        //Comprobamos si el canal requiere contraseña
        if (channel->getPassword() != "" && args[1] != channel->getPassword())
        {
            std::string Msg =  "ERROR: Invalid password\n" ;
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        //Comprobamos si el canal requiere invitacion y si esta invitado
        if (channel->isInviteOnly() && !channel->IsInvited(client.GetClientSocket()))
        {
            std::string Msg =  "ERROR: You are not invited to this channel\n" ;
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        //Comprobar el numero de usuarios en el canal
        if (channel->isLimitUsersEnabled() && channel->GetUsers().size() >= channel->getLimitUsers())
        {
            std::string Msg =  "ERROR: Channel is full\n" ;
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        //Comprobamos si esta ya en el canal
        if (channel->UserExists(client.GetClientSocket()))
        {
            std::string Msg =  "ERROR: You are already in this channel\n" ;
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
        }
        channel->AddUser(client.GetClientSocket());
        std::cout   << Server::getCurrentTime() 
                    << GREEN << "[+] Client <" << client.GetClientSocket() << "> joined channel " 
                    << MAGENTA << channelName << RESET << std::endl;
        std::string Msg =  "You have joined the channel\n" ;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
        return;
    }

    // Si el canal no existe, se crea uno nuevo y añade al cliente como operador
    if (!channelFound)
    {
        Channel newChannel(channelName);
        newChannel.AddUser(client.GetClientSocket());
        newChannel.addOperator(client.GetClientSocket());
        this->channels.push_back(newChannel);
        
        std::cout   << Server::getCurrentTime() 
                    << GREEN << "[+] Client <" << client.GetClientSocket() << "> has created a new channel: " 
                    << MAGENTA << channelName << RESET << std::endl;
        std::cout   << Server::getCurrentTime()
                    << BLUE << "Assigned as Admin of channel: " 
                    << MAGENTA << channelName << RESET 
                    << BLUE << " to Client: "
                    << MAGENTA << client.GetClientSocket() << RESET << std::endl;
        std::string Msg =  "Channel created successfully. You are now "  "Admin\n" ;
        send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
    } */

   // correcion de la funcion
   bool channelFound = ChannelExists(channelName);

    if (channelFound) {

        // Comprobar si el canal requiere contraseña
        Channel* channel = GetThisChannel(channelName);
        if (channel->getPassword() != "" && args.size() > 1 && args[1] != channel->getPassword()) {
            std::string Msg = "ERROR: Invalid password\n";
            send(client.GetClientSocket(), Msg.c_str(), Msg.size(), 0);
            return;
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

// Mensaje de bienvenida
void Server::sendJoinMessages(Client &client, Channel &channel)
{
    std::string nick = client.GetClientNick();
    std::string channelName = channel.GetName();
    std::string serverName = "ft_irc"; // Cambia esto por el nombre de tu servidor

    // Construir el mensaje JOIN
    std::string joinMsg = ":" + nick + "!" + client.GetUsername() + "@" + client.GetHostname() + " JOIN :" + channelName + "\n";
    std::vector<int>::iterator it;
    for (it = channel.GetUsers().begin(); it != channel.GetUsers().end(); ++it) {
        Client* user = GetThisClient(*it);
        if (user) {
            send(user->GetClientSocket(), joinMsg.c_str(), joinMsg.size(), 0);
        }
    }

    // Construir el mensaje RPL_NAMREPLY (353)
    std::string userList = "= " + channelName + " :";
    for (it = channel.GetUsers().begin(); it != channel.GetUsers().end(); ++it) {
        Client* user = GetThisClient(*it);
        if (user) {
            userList += user->GetClientNick() + " ";
        }
    }
    userList += "\n";
    std::string namesMsg = ":" + serverName + " 353 " + nick + " " + userList;

    // Construir el mensaje RPL_ENDOFNAMES (366)
    std::string endNamesMsg = ":" + serverName + " 366 " + nick + " " + channelName + " :End of /NAMES list\n";

    // Enviar los mensajes a todos los usuarios en el canal
    for (it = channel.GetUsers().begin(); it != channel.GetUsers().end(); ++it) {
        Client* user = GetThisClient(*it);
        if (user) {
            send(user->GetClientSocket(), namesMsg.c_str(), namesMsg.size(), 0);
            send(user->GetClientSocket(), endNamesMsg.c_str(), endNamesMsg.size(), 0);
        }
    }
}


Channel* Server::GetCurrentChannel(int clientSocket)
{
    for (size_t i = 0; i < this->channels.size(); i++)
    {
        if (this->channels[i].UserExists(clientSocket))
            return &this->channels[i];
    }
    return nullptr;
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

void Server::handleClientReconnection(int clientSocket) {
    for (std::vector<Client>::iterator it = this->clients.begin(); it != this->clients.end(); ++it) {
        if (it->GetClientSocket() == clientSocket) {
            it->SetDisconnected(false);

            // Procesar los comandos almacenados
            while (!it->getCommandQueue().empty()) {
                std::string command = it->getCommandQueue().front();
                it->getCommandQueue().pop();

                CommandHandler cmdHandler;
                cmdHandler.handleCommand(command, *this, *it);
            }
            break;
        }
    }
}