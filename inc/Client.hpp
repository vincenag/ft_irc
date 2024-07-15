#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Library.hpp"

class Client 
{
    public:
        Client();
        ~Client();
        Client(Client const &src);
        Client &operator=(Client const &src);

        void SetClientSocket(int clientSocket);
        void SetClientIpAddr(std::string clientIpAddr);

        int GetClientSocket() const;
        std::string GetClientIpAddr() const;

        std::string GetClientNick() const;
        void SetClientNick(std::string clientNick);

        bool GetAuthenticated();
        void SetAuthenticated(bool Authenticated);

        // Definir estado de autenticado de USER y NICK
        void SetNickSet(bool value);
        void SetUserSet(bool value);
        bool getUser();
        bool getNick();

        // Metodos para USER
        void SetUsername(const std::string &username);
        void SetRealname(const std::string &realname);
        void SetHostname(const std::string &hostname);
        void SetServername(const std::string &servername);
        bool IsRegistered() const;
        void SetRegistered(bool value);

        std::string GetUsername() const;
        std::string GetRealname() const;
        std::string GetHostname() const;
        std::string GetServername() const;

        // ctrl + D
        std::string &getBuffer();

        // ctrl + Z
        bool IsDisconnected() const;
        void SetDisconnected(bool state);
        std::queue<std::string> getCommandQueue();


    
    private:
        int clientSocket;
        std::string clientIpAddr;
        std::string clientNick;
        bool Authenticated;

        // Definir estado de autenticado de USER y NICK
        bool nickSet;
        bool userSet;

        // atributos para USER
        std::string username;
        std::string realname;
        std::string hostname;
        std::string servername;

        // ctrl + D 
        std::string buffer;

        // ctrl + Z
        bool disconnected;
        std::queue<std::string> commandQueue;
        
        bool registered;
};

#endif