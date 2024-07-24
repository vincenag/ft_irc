/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCBot.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdelicad <rdelicad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/19 17:59:26 by rdelicad          #+#    #+#             */
/*   Updated: 2024/07/24 16:37:33 by rdelicad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRCBot.hpp"

IRCBot::IRCBot(const std::string &server, int port, const std::string &channel, const std::string &nick, const std::string &user, const std::string &password)
    : _server(server), _port(port), _channel(channel), _nick(nick), _user(user), _password(password)
{
    connectToServer();
    joinChannel();
}

void IRCBot::setSocketNonBlocking(int sockfd) 
{
    if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0) {
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

std::string IRCBot::sendMessagesOfDay() 
{
    // Lista de mensajes del día
    static const char* messages[] = {
        "Message of the day: Keep always a positive attitude!",
        "Message of the day: Believe in yourself and all that you are.",
        "Message of the day: Your limitation—it's only your imagination.",
        "Message of the day: Push yourself, because no one else is going to do it for you.",
        "Message of the day: Great things never come from comfort zones.",
        "Message of the day: Dream it. Wish it. Do it.",
        "Message of the day: Success doesn’t just find you. You have to go out and get it.",
        "Message of the day: The harder you work for something, the greater you’ll feel when you achieve it.",
        "Message of the day: Dream bigger. Do bigger.",
        "Message of the day: Don’t stop when you’re tired. Stop when you’re done.",
        "Message of the day: Wake up with determination. Go to bed with satisfaction.",
        "Message of the day: Do something today that your future self will thank you for.",
        "Message of the day: Little things make big days.",
        "Message of the day: It’s going to be hard, but hard does not mean impossible.",
        "Message of the day: Don’t wait for opportunity. Create it.",
        "Message of the day: Sometimes we’re tested not to show our weaknesses, but to discover our strengths.",
        "Message of the day: The key to success is to focus on goals, not obstacles.",
        "Message of the day: Dream it. Believe it. Build it.",
        "Message of the day: Keep going. Everything you need will come to you at the perfect time.",
        "Message of the day: Don’t just dream about success. Get up and make it happen."
    };

    // Calcular el número de mensajes
    int numMessages = sizeof(messages) / sizeof(messages[0]);

    // Seleccionar un mensaje aleatorio
    int index = rand() % numMessages;
    return messages[index];
}