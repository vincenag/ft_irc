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

    private:
        std::string name;
        std::vector<int> users;
        std::set<int> operators; // Almacenamos sockets de los operadores
};

#endif