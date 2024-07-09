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
    
    private:
        int clientSocket;
        std::string clientIpAddr;
        std::string clientNick;
        bool Authenticated;

};

#endif