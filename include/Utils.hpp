#ifndef UTILS_COMMANDS_HPP
#define UTILS_COMMANDS_HPP

#include <string>
#include <cstddef>
#include <iostream>
#include <map>
#include <vector>
#include "Client.hpp"

std::string extractString(const std::string &line);
std::string extractCommand(const std::string &line);
std::vector<std::pair<std::string, std::string> >parseJoinVec(const std::string &line, Client &client);
std::vector<std::string> split(const std::string &s, char delim);
bool checkWihiteSpace(const std::string &line);
std::string trim(const std::string &s);
std::string extractPrivmsgText(const std::string &line);
std::string extractPrivmsgTarget(const std::string &line);
bool nick_exists(const std::string &nick, const std::vector<Client*> &clients);
void check_conncetivity(Client &client);
std::string extractJoinString(const std::string line);

#endif