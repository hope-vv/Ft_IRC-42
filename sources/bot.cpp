#include "../include/Server.hpp"
#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include <iostream>

void IRCServer::createBot() {
    Client *bot = new Client(-1); 
    bot->setNick("Bot13");
    bot->setName("serverBot");
    bot->setAuthenticated(true);
    clients.push_back(bot);
    this->bot = bot;

    std::cout << "[BOT] Bot13 created and registered" << std::endl;

    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
        Channel* ch = it->second;
        if (!ch->hasClient(bot)) {
            ch->addClient(bot);
            bot->addChannel(ch->getName());

            std::string joinMsg = ":" + bot->getNick() + "!" + bot->getName() + "@server JOIN " + ch->getName() + "\r\n";

            const std::vector<Client*>& clientsInChannel = ch->getClients();
            for (size_t i = 0; i < clientsInChannel.size(); ++i) {
                if (clientsInChannel[i] != bot)
                    clientsInChannel[i]->sendMsg(joinMsg);
            }

            std::string namesList;
            for (size_t i = 0; i < clientsInChannel.size(); ++i)
                namesList += clientsInChannel[i]->getNick() + " ";
            
            for (size_t i = 0; i < clientsInChannel.size(); ++i) {
                if (clientsInChannel[i] != bot) {
                    clientsInChannel[i]->sendMsg(
                        ":irc.server 353 " + clientsInChannel[i]->getNick() +
                        " = " + ch->getName() + " :" + namesList + "\r\n"
                    );
                    clientsInChannel[i]->sendMsg(
                        ":irc.server 366 " + clientsInChannel[i]->getNick() +
                        " " + ch->getName() + " :End of /NAMES list\r\n"
                    );
                }
            }
        }
    }
}
