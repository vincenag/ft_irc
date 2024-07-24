/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCBot.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rdelicad <rdelicad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/19 17:54:56 by rdelicad          #+#    #+#             */
/*   Updated: 2024/07/24 16:31:09 by rdelicad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCBOT_HPP
# define IRCBOT_HPP

# include "Library.hpp"

class IRCBot
{
    private:
        std::string _server;
        int _port;
        std::string _channel;
        std::string _nick;
        std::string _user;
        std::string _password;
        int _socket;

        void connectToServer();
        void joinChannel();
        void sendCommand(const std::string &command);
        void setSocketNonBlocking(int sockfd);

    public:
        IRCBot(const std::string &server, int port, const std::string &channel, 
                const std::string &nick, const std::string &user, 
                const std::string &password);
                
        static std::string sendMessagesOfDay();
};

#endif