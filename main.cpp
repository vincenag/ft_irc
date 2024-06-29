#include "Server.hpp"
#include "ArgumentsValidator.hpp"

int main(int argc, char *argv[])
{
    try
    {
        // Validar argumentos
        ArgumentsValidator validator(argc, argv);
        int port = validator.getPort();
        std::string password = validator.getPassword();

        // Inicializar servidor
        Server server;
        signal(SIGINT, Server::signalHandler);

        server.serverInit(port, password);

    }
    catch(const std::exception &e){
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
} 