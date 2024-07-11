#ifndef CHANEL_HPP
#define CHANEL_HPP

#include "Library.hpp"

class Channel 
{
    public:
        Channel();
        Channel(std::string name);
        ~Channel();
        Channel(Channel const &src);
        Channel &operator=(Channel const &src);

        void SetName(std::string name);
        std::string GetName();

        void AddUser(int clientSocket);
        void RemoveUser(int clientSocket);

        std::vector<int> GetUsers();

        bool UserExists(int clientSocket);

        // operador de canal

        bool isOperator(int clientSocket) const;
        void addOperator(int clientSocket);
        void removeOperator(int clientSocket);

        // invitados
        bool isInviteOnly() const;
        void setInviteOnly(bool value);
        void inviteUser(int clientSocket);
        bool IsInvited(int clientSocket) const;

        // bloqueo de topicos
        bool isTopicblock() const;
        void setTopicblock(bool value);
        void setTopic(std::string topic);
        std::string getTopic() const;

        void setPassword(std::string password);
        std::string getPassword() const;

        void setLimitUsers(int limit);
        unsigned int getLimitUsers() const;
        bool isLimitUsersEnabled() const;
        void setLimitUsersEnabled(bool value);

    private:
        std::string name;
        std::vector<int> users;
        std::set<int> operators; // Almacenamos sockets de los operadores

        std::string password;

        bool InviteOnly;
        std::set<int> invitedUsers;

        std::string topic;
        bool Topicblock;

        unsigned int limitUsers;
        bool limitUsersEnabled;
};

#endif