/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCBot.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lxuxer <lxuxer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/19 17:59:26 by rdelicad          #+#    #+#             */
/*   Updated: 2024/07/20 14:27:09 by lxuxer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCBot.hpp"

IRCBot::IRCBot(const std::string &server, int port, const std::string &channel, const std::string &nick, const std::string &user, const std::string &password)
    : _server(server), _port(port), _channel(channel), _nick(nick), _user(user), _password(password)
{
    connectToServer();
    joinChannel();
    //start();
}

void IRCBot::setSocketNonBlocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags < 0) {
        perror("fcntl");
        exit(1);
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("fcntl");
        exit(1);
    }
}

void IRCBot::connectToServer() 
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        exit(1);
    }
    
    setSocketNonBlocking(_socket);
    
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(_port);
    inet_pton(AF_INET, _server.c_str(), &serv_addr.sin_addr);

    if (connect(_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        if (errno != EINPROGRESS) {  // Si no es un error de progreso en curso
            std::cerr << "Connection failed" << std::endl;
            close(_socket);
            exit(1);
        }
    }
    std::cout << "Connected to server" << std::endl;

    // Enviar comandos de autenticación
    sendCommand("PASS " + _password);
    sendCommand("NICK " + _nick);
    sendCommand("USER " + _user);
}

void IRCBot::joinChannel() 
{
    // Unirse al canal
    sendCommand("JOIN " + _channel + "\r\n");
    std::cout << "Joined channel: " << _channel << std::endl;

    // Enviar topic del canal
    sendCommand("TOPIC " + _channel + " :Hello, I am a bot\r\n");
}

void IRCBot::sendCommand(const std::string& command) {
    std::string cmd = command + "\r\n";
    send(_socket, cmd.c_str(), cmd.size(), 0);
    std::cout << "Sent command: " << command << std::endl;
}

void IRCBot::start() 
{
    // Crear el descriptor de epoll
    _epoll_fd = epoll_create1(0);
    if (_epoll_fd < 0) {
        perror("epoll_create1");
        exit(1);
    }

    // Configurar el socket para eventos de lectura
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET; // EPOLLET para modo no bloqueante
    event.data.fd = _socket;

    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _socket, &event) < 0) {
        perror("epoll_ctl");
        close(_epoll_fd);
        exit(1);
    }

    // Crear un buffer para eventos
    struct epoll_event events[10];
    while (true) {
        int nfds = epoll_wait(_epoll_fd, events, 10, 1000); // Esperar hasta 1 segundo
        if (nfds < 0) {
            perror("epoll_wait");
            close(_epoll_fd);
            exit(1);
        }

        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == _socket) {
                char buffer[1024] = {0};
                int valread = recv(_socket, buffer, sizeof(buffer) - 1, 0);
                if (valread < 0) {
                    perror("recv error");
                    close(_epoll_fd);
                    exit(1);
                } else if (valread == 0) {
                    std::cerr << "Connection closed" << std::endl;
                    close(_epoll_fd);
                    exit(1);
                }

                buffer[valread] = '\0'; // Null-terminar el buffer
                std::string message(buffer);
                std::cout << "Received message: " << message << std::endl;

                // Verificar si el mensaje es "start"
                if (message.find("PRIVMSG " + _channel + " :start") != std::string::npos) {
                    sendMessagesOfDay();
                }
            }
        }
    }

    // Cerrar el descriptor de epoll
    close(_epoll_fd);
}

void IRCBot::sendMessagesOfDay() 
{
    // Enviar mensajes del día
    sendCommand("PRIVMSG " + _channel + " :Hello, I am a bot\r\n");
}