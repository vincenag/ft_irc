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

#define HEXCHAT_SERVER "ft_irc"

#define nullptr NULL


#include <iostream>
#include <iomanip>
#include <exception>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <string>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <cerrno>

#include "Client.hpp"
#include "Server.hpp"
#include "ArgumentsValidator.hpp"
#include "Channel.hpp"
#include "CommandHandler.hpp"
#include "Utiles.hpp"
#include "IRCBot.hpp"

#endif