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
    }
    return *this;
}

bool Server::Signal = false;

void Server::signalHandler(int signal)
{
    (void)signal;
    std::cout << "Signal received" << std::endl;
    Signal = true;
}

void Server::serverInit(int port, std::string password)
{
    this->port = port;
    this->password = password;

    std::cout << "Server initialized" << std::endl;

    this->socketInit();

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

/* funcion poll: poll() y la estructura pollfd se utilizan para gestionar de manera eficiente múltiples conexiones de clientes. 
Permiten al servidor monitorear varios descriptores de archivo simultáneamente, reaccionar a eventos de E/S de forma no bloqueante, y manejar nuevas conexiones, datos entrantes y desconexiones de manera ordenada y eficiente. 

Deberia agregar cada socket a la estructura poll, para monitorear cambios

struct pollfd {
 int     fd; //-> file descriptor
 short   events;//-> requested events
 short   revents;//-> returned events
};
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

    //Crear nueva instancia de la clase Client
    
    Client newClient;
    
    newClient.SetClientSocket(clientSocket);
    newClient.SetClientIpAddr(inet_ntoa(clientAddress.sin_addr));

    std::cout << "New client connected. IP: " << newClient.GetClientIpAddr() << std::endl;

    clients.push_back(newClient);
}

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
        /* CommandHandler commandHandler;
        std::string buffStr(buff);
        commandHandler.handleCommand(this->clients[0], buff); */

        //Agregar la logica para manejar los datos recibidos (parseo, validacion, etc.)
    }
}
