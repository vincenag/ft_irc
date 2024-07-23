#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Library.hpp"

class Client 
{
    public:

        // *********** CONSTRUCTORS ***********

        Client();
        ~Client();
        Client(Client const &src);
        Client &operator=(Client const &src);

        // *********** GETTERS ***********

        void SetClientSocket(int clientSocket);
        void SetClientIpAddr(std::string clientIpAddr);

        int GetClientSocket() const;
        std::string GetClientIpAddr() const;

        std::string GetClientNick() const;
        void SetClientNick(std::string clientNick);

        bool GetAuthenticated();
        void SetAuthenticated(bool Authenticated);

        // *********** CURRENT CHANNEL ***********
       
        std::string GetChannel() const;
        void SetChannel(const std::string &channel);

        // *********** NICK ***********

        void SetNickSet(bool value);
        void SetUserSet(bool value);
        bool getUser();
        bool getNick();
        bool isValidNick(const std::string &nick);

        // *********** USER ***********

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

        // *********** BUFFER CTRL + D ***********
        std::string &getBuffer();
        void clearBuffer();

        // *********** DISCONNECTED CTRL + Z ***********
        bool IsDisconnected() const;
        void SetDisconnected(bool state);
        std::queue<std::string> getCommandQueue();


    
    private:
        int clientSocket;
        std::string clientIpAddr;
        std::string clientNick;
        bool Authenticated;
        

        // *********** NICK ***********
        bool nickSet;
        bool userSet;

        // *********** USER ***********
        std::string username;
        std::string realname;
        std::string hostname;
        std::string servername;

        // *********** BUFFER CTRL + D ***********
        std::string buffer;

        // *********** DISCONNECTED CTRL + Z ***********
        bool disconnected;
        std::queue<std::string> commandQueue;
        
        // *********** REGISTERED ***********
        bool registered;

        // *********** Current Channel ***********
        std::string currentChannel;
};

#endif