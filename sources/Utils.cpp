#include "../include/Utils.hpp"

std::string extractString(const std::string &line)
{
    std::string nick = line;

    nick.erase(0, nick.find_first_not_of(" \t\r\n"));

    if (nick.size() > 4)
        nick = nick.substr(4);
    else
        return "";

    nick.erase(0, nick.find_first_not_of(" \t\r\n"));
    size_t spacePos = nick.find(' ');

    if (spacePos != std::string::npos)
        nick = nick.substr(0, spacePos);

    nick.erase(nick.find_last_not_of(" \t\r\n") + 1);

    return nick;
}

std::string extractCommand(const std::string &line)
{
    std::string trimmed = line;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));

    std::string cmd;
    size_t spacePos = trimmed.find(' ');
    if (spacePos != std::string::npos)
        cmd = trimmed.substr(0, spacePos);
    else
        cmd = trimmed;

    return cmd;  
}

std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    std::string item;
    for (size_t i = 0; i < s.size(); ++i)
    {
        if (i == s.size() || s[i] == delim)
        {
            elems.push_back(item);
            item.clear();
        }
        else
            item += s[i];
    }
    elems.push_back(item);
    return elems;
}

bool isValidChannel(const std::string &chan)
{
    if (chan.empty()) return false;
    if (chan[0] != '#' && chan[0] != '&' && chan[0] != '!') return false;
    if (chan.size() == 1) return false;
    if (chan.size() > 50) return false;
    size_t i = 1;
    while (i < chan.size())
    {
        if (chan[i] == ';' || chan[i] == 7)
            return false;
        i++;
    }
    return true;
}

bool checkWihiteSpace(const std::string &line)
{
    size_t i = 0;
    int count = 0;
    while (i < line.size())
    {
        if (line[i] == ' ')
        {
            if (line[i + 1] && line[i + 1] == ',')
                return (false);
            else
                count++;
        }
        else if (line[i] == ',')
        {
            if (line[i + 1] && line[i + 1] && line[i + 1] == ' ')
                return (false);
        }
        i++;
    }
    if (count > 1 || line[i - 1] == ',')
        return false;
    return (true);
}

std::vector<std::pair<std::string, std::string> >parseJoinVec(const std::string &line, Client &client)
{
    (void)client;
    std::string msg;
    std::vector<std::pair<std::string, std::string> > result;
    if (!checkWihiteSpace (line))
    {
        msg = ":irc.server 461 JOIN : Wrong spaces\r\n";
        client.sendMsg(msg);
        return result;
    }

    size_t spacePos = line.find(' ');
    std::string channelsPart = line.substr(0, spacePos);
    std::string keysPart;
    if (spacePos != std::string::npos)
        keysPart = line.substr(spacePos + 1);

    if (channelsPart.empty())
    {
        msg = ":irc.server 461 JOIN : Not enough paramaters\r\n";
        client.sendMsg(msg);
        return result;
    }
    std::vector<std::string> channels = split(channelsPart, ',');
    std::vector<std::string> keys;
    if (!keysPart.empty())
        keys = split(keysPart, ',');

    for (size_t i = 0; i < channels.size(); ++i)
    {
        const std::string &chan = channels[i];
        if (!isValidChannel(chan))
        {
            msg = ":irc.server 403 " + chan + " :No such channel\r\n";
            client.sendMsg(msg);
            continue;
        }
        std::string key = (i < keys.size()) ? keys[i] : "";
        if (!key.empty() && key.find(' ') != std::string::npos)
        {
            msg = ":irc.server 475 " + chan + " :Bad channel key\r\n";
            client.sendMsg(msg);
            continue;
        }
        result.push_back(std::make_pair(channels[i], key));
    }

    return result;
}

std::string trim(const std::string &s)
{
    size_t start = 0;
    while (start < s.size() && (s[start] == ' ' || s[start] == '\t'))
        ++start;
    if (start == s.size())
        return "";

    size_t end = s.size() - 1;
    while (end > start && (s[end] == ' ' || s[end] == '\t'))
        --end;
    return s.substr(start, end - start + 1);
}

std::string extractPrivmsgTarget(const std::string &line)
{
    std::string tmp = line;
    tmp.erase(0, tmp.find_first_not_of(" \t\r\n"));

    std::string cmd = tmp.substr(0, 5);

    if (cmd != "PRIVM")
        return "";

    if (tmp.size() > 7)
        tmp = tmp.substr(7);
    else
        return "";

    tmp.erase(0, tmp.find_first_not_of(" \t\r\n"));
    size_t spacePos = tmp.find(' ');
    if (spacePos != std::string::npos)
        tmp = tmp.substr(0, spacePos);

    return tmp;
}
std::string extractPrivmsgText(const std::string &line)
{
    size_t colonPos = line.find(" :");
    if (colonPos == std::string::npos)
        return "";
    return line.substr(colonPos + 2);
}

bool nick_exists(const std::string &nick, const std::vector<Client*> &clients)
{
    size_t i = 0;

    while (i < clients.size())
    {
        if (clients[i]->getNick() == nick)
            return (true);
        i++;
    }
    return (false);
}
void check_conncetivity(Client &client)
{
    if (client.isRegistred())
    {
        std::string msg = client.getNick() + " :Welcome to the IRC server!\r\n";
        client.sendMsg(":irc.server 001 " + client.getNick() + " :Welcome to the IRC server\r\n");
        client.sendMsg(":irc.server 002 " + client.getNick() + " :Your host is irc.server\r\n");
        client.sendMsg(":irc.server 003 " + client.getNick() + " :This server was created just now\r\n");
        client.sendMsg(":irc.server 004 " + client.getNick() + " irc.server\r\n");
        client.sendMsg(msg);
    }
}

std::string extractJoinString(const std::string line)
{
    std::string joinLine = trim(line);
    if (joinLine.size() <= 5)
        return "";
    joinLine = joinLine.substr(5, joinLine.size());
    return (trim(joinLine));
}
