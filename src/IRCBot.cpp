/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCBot.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lxuxer <lxuxer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/19 17:59:26 by rdelicad          #+#    #+#             */
/*   Updated: 2024/07/20 13:18:38 by lxuxer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCBot.hpp"

IRCBot::IRCBot(const std::string &server, int port, const std::string &channel, const std::string &nick, const std::string &user, const std::string &password)
    : _server(server), _port(port), _channel(channel), _nick(nick), _user(user), _password(password)
{
    connectToServer();
    joinChannel();
    //sendMessagesOfDay();
}

void IRCBot::connectToServer() 
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        exit(1);
    }
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(_port);
    inet_pton(AF_INET, _server.c_str(), &serv_addr.sin_addr);

    if (connect(_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        exit(1);
    }
    std::cout << "Connected to server" << std::endl;

    // Enviar comando PASS si es necesario
    sendCommand("PASS " + _password);

    // Enviar comando NICK
    sendCommand("NICK " + _nick);

    // Enviar comando USER
    sendCommand("USER " + _user);
}

void IRCBot::joinChannel() {
    sendCommand("JOIN " + _channel + "\r\n");
    std::cout << "Joined channel: " << _channel << std::endl;
}

void IRCBot::sendMessagesOfDay() 
{
    while (true) {
        std::string message = "PRIVMSG " + _channel + " :Mensaje del día: ¡Que tengas un excelente día!";
        sendCommand(message);
        std::cout << "Sent message: " << message << std::endl;
        // enviar cada 2 minutos
        sleep(120); 
    }
}

void IRCBot::sendCommand(const std::string& command) {
    std::string cmd = command + "\r\n";
    send(_socket, cmd.c_str(), cmd.size(), 0);
    std::cout << "Sent command: " << command << std::endl;
}

void IRCBot::start() 
{
    sendMessagesOfDay();
}