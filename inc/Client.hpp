#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>

class Client {
    public:
        Client();
        ~Client();
        Client(Client const &src);
        Client &operator=(Client const &src);

        void SetClientSocket(int clientSocket);
        void SetClientIpAddr(std::string clientIpAddr);

        int GetClientSocket();
        std::string GetClientIpAddr();
    
    private:
        int clientSocket;
        std::string clientIpAddr;


};

#endif