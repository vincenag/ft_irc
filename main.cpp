#include "Library.hpp"

static void printIRCChatBanner()
{
    std::cout << RED << "  8 8888 8 888888888o.       ,o888888o.                  ,o888888o.    8 8888        8          .8.          8888888 8888888888" << std::endl;
    std::cout << GREEN << "  8 8888 8 8888    `88.     8888     `88.               8888     `88.  8 8888        8         .888.               8 8888" << std::endl;
    std::cout << YELLOW << "  8 8888 8 8888     `88  ,8 8888       `8.           ,8 8888       `8. 8 8888        8        :88888.              8 8888" << std::endl;
    std::cout << BLUE << "  8 8888 8 8888     ,88  88 8888                     88 8888           8 8888        8       . `88888.             8 8888" << std::endl;
    std::cout << MAGENTA << "  8 8888 8 8888.   ,88'  88 8888                     88 8888           8 8888        8      .8. `88888.            8 8888" << std::endl;
    std::cout << CYAN << "  8 8888 8 888888888P'   88 8888                     88 8888           8 8888        8     .8`8. `88888.           8 8888" << std::endl;
    std::cout << RESET << "  8 8888 8 8888`8b       88 8888                     88 8888           8 8888888888888    .8' `8. `88888.          8 8888" << std::endl;
    std::cout << RESET << "  8 8888 8 8888 `8b.     `8 8888       .8'           `8 8888       .8' 8 8888        8   .8'   `8. `88888.         8 8888" << std::endl;
    std::cout << RESET << "  8 8888 8 8888   `8b.      8888     ,88'               8888     ,88'  8 8888        8  .888888888. `88888.        8 8888" << std::endl;
    std::cout << RESET << "  8 8888 8 8888     `88.     `8888888P'                  `8888888P'    8 8888        8 .8'       `8. `88888.       8 8888" << std::endl;

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