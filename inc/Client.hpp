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
        bool IsFullyAuthenticated() const;

        // Metodos para USER
        void SetUsername(const std::string &username);
        void SetRealname(const std::string &realname);
        void SetHostname(const std::string &hostname);
        void SetServername(const std::string &servername);
    
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

};

#endif