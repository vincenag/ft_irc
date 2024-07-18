#include "Utiles.hpp"

void Utiles::sendWelcomeMessage(Client& client) 
{
    //mensaje bienvenida segun protoloco IRC
    Utiles::sendNumericReply(client, 001, "Welcome to the Internet Relay Network " + client.GetClientNick());
    Utiles::sendNumericReply(client, 002, "Your host is ft_irc, running version 1.0");
    Utiles::sendNumericReply(client, 003, "This server was created on 18/07/224");
    Utiles::sendNumericReply(client, 004, "ft_irc version 1.0");

    // server info
    /* Utiles::sendNumericReply(client, 005, "CHANTYPES=# PREFIX=(ov)@+ NETWORK=ft_irc CASEMAPPING=ascii");
    Utiles::sendNumericReply(client, 251, "There are 1 users and 0 services on 1 servers");
    Utiles::sendNumericReply(client, 252, "0 operator(s) online");
    Utiles::sendNumericReply(client, 253, "0 unknown connection(s)");
    Utiles::sendNumericReply(client, 254, "0 channels formed");
    Utiles::sendNumericReply(client, 255, "I have 1 clients and 0 servers");
    Utiles::sendNumericReply(client, 265, "Current Local Users: 1  Max: 1");
    Utiles::sendNumericReply(client, 266, "Current Global Users: 1  Max: 1"); */

    // MOTD
    /* Utiles::sendNumericReply(client, 375, "Message of the day -");
    Utiles::sendNumericReply(client, 372, "Welcome to ft_irc, the best IRC server ever");
    Utiles::sendNumericReply(client, 376, "End of /MOTD command."); */
}

void Utiles::sendNumericReply(Client& client, int code, const std::string& message) {
    std::string formattedMessage = ":ft_irc " + Utiles::toString(code) + " " + client.GetClientNick() + " " + message + "\r\n";
    send(client.GetClientSocket(), formattedMessage.c_str(), formattedMessage.size(), 0);
}

std::string Utiles::toString(int value) {
    std::ostringstream ss;
    ss << value;
    return ss.str();
}