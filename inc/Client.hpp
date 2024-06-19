#ifndef CLIENT_HPP
#define CLIENT_HPP

class Client {
    public:
        Client();
        ~Client();
        Client(Client const &src);
        Client &operator=(Client const &src);
};

#endif