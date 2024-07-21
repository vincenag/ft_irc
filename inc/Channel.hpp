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

        // usuarios
        bool UserExists(int clientSocket);
        bool IsUserInChannel(int clientSocket) const;

        // operador de canal
        void addOperator(int clientSocket);
        void removeOperator(int clientSocket);
        bool IsOperator(int clientSocket) const;
        void SetOperator(int clientSocket, bool isOperator);


        // invitados
        bool isInviteOnly() const;
        void setInviteOnly(bool value);
        void inviteUser(int clientSocket);
        bool IsInvited(int clientSocket) const;

        // comando Kick
        void KickUser(int userSocket, const std::string &reason);
        void BroadcastMessage(const std::string& message, Server& server);

        // bloqueo de topicos
        bool isTopicblock() const;
        void setTopicblock(bool value);
        void setTopic(const std::string &topic);
        const std::string &getTopic() const;
        bool isTopicChanged(const std::string &topic) const;
        bool HasTopic() const;

        // Modos del canal
        // +k: clave
        bool isModeKEnabled() const;
        void setModeKEnabled(bool value);

        void setPassword(std::string password);
        std::string getPassword() const;

        void setLimitUsers(int limit);
        unsigned int getLimitUsers() const;
        bool isLimitUsersEnabled() const;
        void setLimitUsersEnabled(bool value);

    private:
        std::set<char> modes; // Modos del canal
        std::string name;
        std::vector<int> users;
        std::set<int> operators; // Almacenamos sockets de los operadores

        std::string password;

        bool InviteOnly;
        std::set<int> invitedUsers;

        std::string topic;
        bool Topicblock;

        // Modo +l
        unsigned int limitUsers;
        bool limitUsersEnabled;

        // Modo +k
        bool modeK;
};

#endif