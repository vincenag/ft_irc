#include "Server.hpp"

Server::Server(){this->serverSocket = -1;}

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
    std::cout << "Signal received" << std::endl;
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

    std::cout << "Server initialized" << std::endl;

    this->socketInit();

    Channel chanelGeneral("General");
    this->channels.push_back(chanelGeneral);

    while (Server::Signal == false)
    {
        int ret = poll(&fds[0], fds.size(), -1); // -1: Esperar indefinidamente hasta que se produzca un evento
        if (ret < 0)
            throw std::runtime_error("poll() failed");

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

    std::cout << "Server listening on port " << this->port << std::endl;
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

    std::cout << "New client connected. IP: " << newClient.GetClientIpAddr() << std::endl;

    clients.push_back(newClient);

    //Agregar este cliente al chanel General
    //this->channels[0].AddUser(newClient.GetClientIpAddr());
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
        std::cout << "Client <" << clientSocket << "> Disconnected" << std::endl;
        
        // Limpiar la información del cliente y cerrar el socket del cliente
        close(clientSocket);
        
        // Eliminar el cliente del vector de clientes
        for (size_t i = 0; i < this->clients.size(); ++i) {
            if (this->clients[i].GetClientSocket() == clientSocket) {
                this->clients.erase(this->clients.begin() + i);
                break;
            }
        }     
        // Eliminar el `pollfd` correspondiente del vector `fds`
        for (size_t i = 0; i < this->fds.size(); ++i) {
            if (this->fds[i].fd == clientSocket) {
                this->fds.erase(this->fds.begin() + i);
                break;
            }
        }
        close(clientSocket);
    } else { // Imprimir los datos recibidos
        buff[bytes] = '\0';
        std::cout << "Client <" << clientSocket << "> Data: " << buff << std::endl;
        
        // Buscar el objeto Client correspondiente al clientSocket
    //     for (size_t i = 0; i < this->clients.size(); ++i) {
    //         if (this->clients[i].GetClientSocket() == clientSocket) {
    //             // Convertir el buffer en string
    //             std::string data(buff);
    //             // Crear una instancia de CommandHandler
    //             CommandHandler cmdHandler;
    //             // Llamar a la función para manejar los datos recibidos
    //             cmdHandler.handleClientData(data, *this, this->clients[i]);
    //             break;
    //         }
    //     }
    // }
}