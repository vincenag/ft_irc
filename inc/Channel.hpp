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

        void AddUser(std::string user);
        void RemoveUser(std::string user);

        std::vector<std::string> GetUsers();
    
    private:
        std::string name;
        std::vector<std::string> users;
};

#endif