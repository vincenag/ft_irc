#include "Library.hpp"

static void printIRCChatBanner()
{
   
    std::cout << BLUE << "=================================" << RESET << std::endl;
    std::cout << BLUE << "||" << RESET << YELLOW << "     IRC Chat Server Init    " << BLUE << "||" << RESET << std::endl;
    std::cout << BLUE << "||" << RESET << RED << "     Version 1.0.0           " << BLUE << "||" << RESET << std::endl;
    std::cout << BLUE << "=================================" << RESET << std::endl;
    std::cout << BLUE << "||" << RESET << CYAN << " Author: " << GREEN << "agvincen & rdelicad " << BLUE << "||" << RESET << std::endl;
    std::cout << BLUE << "=================================" << RESET << std::endl;
   

    std::cout << std::endl;
}

int main(int argc, char *argv[])
{
    try
    {
        // Validar argumentos
        ArgumentsValidator validator(argc, argv);
        int port = validator.getPort();
        std::string password = validator.getPassword();
        printIRCChatBanner();

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