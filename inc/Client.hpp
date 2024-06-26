#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>

class Client 
{
    public:
        Client();
        ~Client();
        Client(Client const &src);
        Client &operator=(Client const &src);

        void SetClientSocket(int clientSocket);
        void SetClientIpAddr(std::string clientIpAddr);

        int GetClientSocket();
        std::string GetClientIpAddr();

        // Metodos para los comandos
        void setNickname(std::string nickname);
        std::string getNickname();
        void setUsername(std::string username);
        std::string getUsername();
    
    private:
        int clientSocket;
        std::string clientIpAddr;

        std::string nickname;
        std::string username;
};

#endif