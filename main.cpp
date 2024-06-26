#include "Server.hpp"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <port number> <password>" << std::endl;
        return 1;
    }
    try{
        Server server;
        signal(SIGINT, Server::signalHandler);

        server.serverInit(atoi(argv[1]), argv[2]);

    }
    catch(const std::exception &e){
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
} 