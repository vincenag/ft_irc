#include "ArgumentsValidator.hpp"

ArgumentsValidator::ArgumentsValidator(int argc, char **argv)
{
    parseArguments(argc, argv);
}

void ArgumentsValidator::parseArguments(int argc, char **argv)
{
    if (argc != 3)
    {
        throw std::runtime_error("Usage: <port number> <password>");
    }

    validatePort(argv[1]);
    validatePassword(argv[2]);
}

void ArgumentsValidator::validatePort(const std::string &portStr)
{
    // Verificar que el puerto sea un número
    for (size_t i = 0; i < portStr.size(); i++)
    {
        if (!isdigit(portStr[i]))
        {
            throw std::runtime_error("Port must be a number");
        }
    }

    // Vonvertir el string a int y validar rango
    long portNum = atol(portStr.c_str());
    if (portNum < 1024 || portNum > 49151)
    {
        throw std::runtime_error("Port must be between 1024 and 49151");
    }
    
    port = static_cast<int>(portNum);
}

void ArgumentsValidator::validatePassword(const std::string &password)
{
    if (password.size() < 4)
    {
        throw std::runtime_error("Password must be at least 4 characters long");
    }

    this->password = password;
}

int ArgumentsValidator::getPort() const
{
    return port;
}

std::string ArgumentsValidator::getPassword() const
{
    return password;
}
