#ifndef ServerState_HPP
#define ServerState_HPP

#include "Library.hpp"
class Server;

class ServerState
{
    private:
        int _serverSocket; // Almacenar el socket del servidor
        std::unordered_map<int, std::string> _clients; // Clave: Socket del cliente, Valor: Nick
        std::unordered_map<std::string, std::vector<int>> _channels; // Clave: Nombre del canal, Valor: Lista de sockets de clientes
    public:
        ServerState(int serverSocket);
        // Actualizar el estado cuando un cliente se conecta o desconecta
        void clientConnected(int socket, std::string nick);
        void clientDisconnected(int socket);

        // Actualizar el estado cuando un canal se crea o se destruye
        void channelCreated(std::string channel);
        void channelDestroyed(std::string channel);

        // Actualizar el estado cuando un cliente se subscribe o se desubscribe a un canal
        void clientSubscribed(int socket, std::string channel);
        void clientUnsubscribed(int socket, std::string channel);

        // Obtener el nick de un cliente dado su socket
        std::string getNick(int socket);

        // Obtener la lista de sockets de los clientes suscritos a un canal
        std::vector<int> getChannelClients(std::string channel);

        // Obtener la lista de canales a los que un cliente está suscrito
        std::vector<std::string> getClientChannels(int socket);

        // Obtener la lista de canales existentes
        std::vector<std::string> getChannels();

        // Verificar si un cliente está suscrito a un canal
        bool isClientSubscribed(int socket, std::string channel);

        // Verificar si un canal existe
        bool channelExists(std::string channel);

        // Verificar si un cliente existe
        bool clientExists(int socket);

        // Verificar si un nick está en uso
        bool nickInUse(std::string nick);

        // Obtener el socket de un cliente dado su nick
        int getSocket(std::string nick);

        // Obtener el número de clientes conectados
        int getNumClients();

        // Obtener el número de canales existentes
        int getNumChannels();
};


}