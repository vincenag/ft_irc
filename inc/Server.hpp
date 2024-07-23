#ifndef SERVER_HPP
#define SERVER_HPP

#include "Library.hpp"
class Client;
class Channel;
class IRCBot;

class Server
{
    public:
        Server();
        ~Server();
        Server(Server const &src);
        Server &operator=(Server const &src);

        static void signalHandler(int signal);
        void serverInit(int port, std::string password);

        void JoinChannel(Client &client, std::string channelName, const std::vector<std::string> &args);

        std::string GetPassword();
        void RemoveClient(int clientSocket);

        std::vector<Client> GetClients();
        int GetSocketByNick(const std::string& nick) const;
        Client& GetClientBySocket(int socketId);
        std::vector<Channel> GetChannels();
        Channel* GetThisChannel(std::string channelName);
        Client* GetUserByNick(const std::string& nick);
        bool isUser(const std::string &nick) const;
        int GetSocket(const std::string &nick) const;

        // Mensajes para el cliente hexchat
        void sendJoinMessages(Client &client, Channel &channel);
        void broadcastToChannel(Channel &channel, const std::string &message, int clientSocket);
        void updateUserList(Channel &channel);

        // Metodo para verificar si un canal existe
        bool ChannelExists(std::string channelName);

        // Metodo para obtener un cliente basado en su socket
        Client* GetThisClient(int clientSocket);

        // miembro para obtener fecha y hora actuales
        static std::string getCurrentTime();

        // Miembro para el ctrl + Z
        void handleClientReconnection(int clientSocket);

        //Miembro para el ctrl + C
        void shutdownServer();

        // Miembro para obtener lista de canales
        std::vector<Channel> GetAllChannels() const;

    private:
        std::string password;
        int port;
        static bool Signal;
        int serverSocket; // File descriptor for the server socket
        struct sockaddr_in address; // Server address
        std::vector<struct pollfd> fds; // Vector of file descriptors for the clients
        struct pollfd new_poll; // File descriptor for the new client
        std::vector<Client> clients; // Vector of clients
        std::vector<Channel> channels; // Vector of chanels

        //std::map<int, std::string> clientNicknames;  Map of client nicknames

        void socketInit();
        void acceptClient();
        void getClientdata(int clientSocket);

        // banner para cada cliente que se conecte
        static void printIRCChatBanner(int clientSocket);
};

#endif