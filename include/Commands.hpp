#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "Channel.hpp"
#include "Client.hpp"
#include "Utils.hpp"
#include <vector>
#include <string>
#include <map>

void handleCommand(Client &client, const std::string &line, const std::string &serverPass, const std::vector<Client*> &clients, std::map<std::string, Channel*> &channels, Client *bot);
void handlePass(Client &client, const std::string &line, const std::string &sreverPass);
void handleNick(Client &client, const std::string &line, const std::vector<Client*> &clients);
void handleUser(Client &client, const std::string &line);
void handleJoin(Client &client, const std::string &line, std::map<std::string, Channel*> &channels, Client* bot);
void handlePrivmsg(Client &sender, const std::string &line, const std::vector<Client*> &clients, 
                   std::map<std::string, Channel*> &channels, Client* bot);

#endif