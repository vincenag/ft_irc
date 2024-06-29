#ifndef  ARGUMENTSVALIDATOR_HPP
#define ARGUMENTSVALIDATOR_HPP

#include <string>
#include <stdexcept>
#include <cstdlib>

class ArgumentsValidator 
{
    private:
        int port;
        std::string password;
        void parseArguments(int argc, char **argv);
        void validatePort(const std::string &portStr);
        void validatePassword(const std::string &password);
    public:
        ArgumentsValidator(int argc, char **argv);
        int getPort() const;
        std::string getPassword() const;
};

#endif 