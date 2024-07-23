#include "Utiles.hpp"

void Utiles::sendWelcomeMessage(Client& client) 
{
    //mensaje bienvenida segun protoloco IRC
    Utiles::sendNumericReply(client, 001, "Welcome to the Internet Relay Network " + client.GetClientNick());
    Utiles::sendNumericReply(client, 002, "Your host is ft_irc, running version 1.0");
    Utiles::sendNumericReply(client, 003, "This server was created on 18/07/2024");
    Utiles::sendNumericReply(client, 004, "ft_irc version 1.0");
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