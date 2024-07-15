#include "Utiles.hpp"

void Utiles::sendWelcomeMessage(Client& client) {
    const char* welcomeMessages[] = {
        "Welcome to the IRC server!\r\n",
        "Your registration is complete.\r\n",
    };
    size_t numMessages = sizeof(welcomeMessages) / sizeof(welcomeMessages[0]);

    for (size_t i = 0; i < numMessages; ++i) {
        send(client.GetClientSocket(), welcomeMessages[i], strlen(welcomeMessages[i]), 0);
    }
}

void Utiles::sendNumericReply(Client& client, int code, const std::string& message) {
    std::string formattedMessage = Utiles::toString(code) + " " + message + "\r\n";
    send(client.GetClientSocket(), formattedMessage.c_str(), formattedMessage.size(), 0);
}

std::string Utiles::toString(int value) {
    std::ostringstream ss;
    ss << value;
    return ss.str();
}