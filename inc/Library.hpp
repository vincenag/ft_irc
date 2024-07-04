#ifndef LIBRARY_HPP
#define LIBRARY_HPP

//COLORS
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define RESET "\033[0m"


#define nullptr NULL

#include <iostream>
#include <string>
#include <csignal>
#include <exception>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <vector>
#include <fcntl.h>
#include <arpa/inet.h>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <map>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <iomanip>

#include "Client.hpp"
#include "Server.hpp"
#include "ArgumentsValidator.hpp"
#include "Channel.hpp"
#include "CommandHandler.hpp"

#endif