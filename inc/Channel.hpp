#ifndef CHANEL_HPP
#define CHANEL_HPP

#include <iostream>
#include <vector>
#include <algorithm>


class Channel {
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
    
    private:
        std::string name;
        std::vector<int> users;
};

#endif