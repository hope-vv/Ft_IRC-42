#include "../include/Commands.hpp"
#include <sstream>

void handlePass(Client &client, const std::string &line, const std::string &sreverPass)
{
    std::string msg;

    if (client.isAuthenticated())
    {
        msg = ":irc.server 462 " + client.getNick() + " :You already registred\r\n";
        client.sendMsg(msg);
        return ;
    }
    if (line.empty())
    {
        msg = ":irc.server 461 PASS :Not enough parameters\r\n";
        client.sendMsg(msg);
        return;
    }
    if (line == sreverPass)
    {
        client.setAuthenticated(true);
        msg = ":irc.server NOTICE " + client.getNick() + " :Password accepted\r\n";
        client.sendMsg(msg);
    }
    else
    {
        client.setAuthenticated(false);
        msg = ":irc.server NOTICE " + client.getNick() + " :Password incorrect\r\n";
        client.sendMsg(msg);
    }
}

void handleNick(Client &client, const std::string &line, const std::vector<Client*> &clients)
{
    std::string msg;

    if (line.empty())
    {
        msg = ":irc.server 431 * :No nickname given\r\n";
        client.sendMsg(msg);
        return ;
    }
    else if (nick_exists(line, clients))
    {
        std::string msg = ":irc.server 433 * " + line + " :Nickname is already in use\r\n";
        client.sendMsg(msg);
        return;
    }
    client.setNick(line);
    client.sendMsg("Your nickname has been set successfully to: " + client.getNick() + "\r\n");
    check_conncetivity(client);
}

void handleUser(Client &client, const std::string &line)
{
    std::string name = line;
    std::string msg;

    if (name.empty())
    {
        msg = ":irc.server 431 * :No username given\r\n";
        client.sendMsg(msg);
        return;
    }
    if (!client.getName().empty())
    {
        std::string msg = ":irc.server 462 " + client.getNick() + " :USER name already registred\r\n";
        client.sendMsg(msg);
        return;
    }
    client.setName(name);
    client.sendMsg("Your username has been set successfully to: " + client.getName() + "\r\n");
    check_conncetivity(client);
}

bool validateJoin(Client &client, Channel* ch, const std::string &channelName, const std::string &key)
{
    if (ch->isInviteOnly() && !ch->isInvited(client.getNick()))
    {
        client.sendMsg(":irc.server 473 " + client.getNick() + " " + channelName + " :Cannot join channel (+i)\r\n");
        return false;
    }
    if (ch->isKeySet() && (key.empty() || key != ch->getKey()))
    {
        client.sendMsg(":irc.server 475 " + client.getNick() + " " + channelName + " :Cannot join channel (+k)\r\n");
        return false;
    }
    if (ch->getClients().size() >= ch->getLimit() && ch->getLimit() > 0)
    {
        client.sendMsg(":irc.server 471 " + client.getNick() + " " + channelName + " :Channel is full\r\n");
        return false;
    }
    return true;
}

void joinChannel(Client &client, Channel* ch, const std::string &channelName, Client* bot)
{
    if (!ch->hasClient(&client))
    {
        ch->addClient(&client);
        client.addChannel(channelName);
    }

    std::string joinMsg = ":" + client.getNick() + "!" + client.getName() + "@server JOIN " + channelName + "\r\n";
    for (size_t j = 0; j < ch->getClients().size(); j++)
    {
        if (ch->getClients()[j] != bot)
            ch->getClients()[j]->sendMsg(joinMsg);
    }

    if (bot && !ch->hasClient(bot))
    {
        ch->addClient(bot);
        bot->addChannel(channelName);

        std::string botJoinMsg = ":" + bot->getNick() + "!" + bot->getName() + "@server JOIN " + channelName + "\r\n";
        for (size_t j = 0; j < ch->getClients().size(); j++)
        {
            if (ch->getClients()[j] != bot)
                ch->getClients()[j]->sendMsg(botJoinMsg);
        }
    }
}
void sendNamesReply(Client &client, Channel* ch, const std::string &channelName)
{
    std::string namesMsg = ":irc.server 353 " + client.getNick() + " = " + channelName + " :";
    for (size_t j = 0; j < ch->getClients().size(); j++)
        namesMsg += ch->getClients()[j]->getNick() + " ";
    namesMsg += "\r\n";

    client.sendMsg(namesMsg);
    client.sendMsg(":irc.server 366 " + client.getNick() + " " + channelName + " :User list complete\r\n");
}

void handleJoin(Client &client, const std::string &line, std::map<std::string, Channel*> &channels, Client* bot)
{
    if (!client.isRegistred())
    {
        client.sendMsg(":irc.server 451 :You have not registered yet\r\n");
        return;
    }

    std::vector<std::pair<std::string, std::string> > joinVec = parseJoinVec(line, client);
    for (size_t i = 0; i < joinVec.size(); ++i)
    {
        std::string channelName = joinVec[i].first;
        std::string key = joinVec[i].second;
        Channel* ch;

        if (channels.find(channelName) == channels.end())
        {
            ch = new Channel(channelName);
            channels[channelName] = ch;
            ch->addOperator(client.getNick());
        }
        else
            ch = channels[channelName];

        if (!validateJoin(client, ch, channelName, key))
            continue;

        joinChannel(client, ch, channelName, bot);
        sendNamesReply(client, ch, channelName);
    }
}

void handlePrivmsg(Client &sender, const std::string &line, const std::vector<Client*> &clients, std::map<std::string, Channel*> &channels, Client* bot)
{
    std::string reciev = extractPrivmsgTarget(line);
    std::string msg = extractPrivmsgText(line);

    if (reciev.empty())
    {
        sender.sendMsg(":irc.server 411 " + sender.getNick() + " :No recipient given\r\n");
        return ;
    }
    if (msg.empty())
    {
        sender.sendMsg(":irc.server 412 " + sender.getNick() + " :No text to send\r\n");
        return;
    }
    std::vector<std::string> targetList = split(reciev, ',');
    size_t i = 0;
    size_t j = 0;
    std::string target;
    std::string message;
    bool found = false;
    while (i < targetList.size())
    {
        target = trim(targetList[i]);
        Channel *ch;
        if (!target.empty() && (target[0] == '#' || target[0] == '&' || target[0] == '!' ))
        {
            if (channels.find(target) == channels.end())
            {
                sender.sendMsg(":irc.server 403 " + sender.getNick() + " " + target + " :No such channel\r\n");
                i++;
                continue;
            }
            ch = channels[target];
            message = ":" + sender.getNick() + " PRIVMSG " + target + " :" + msg + "\r\n";
            j = 0;
            while (j < ch->getClients().size())
            {
                if (ch->getClients()[j] != &sender && (ch->getClients()[j] != bot))
                    ch->getClients()[j]->sendMsg(message);
                j++;
            }
            if (bot && !msg.empty() && msg[0] == '!')
            {
                std::string botReply;
                 if (msg == "!hello")
                    botReply = "Hello " + sender.getNick() + "! You called me.";
                else if (msg == "!time") {
                    time_t now = time(0);
                    botReply = "Server time: " + std::string(ctime(&now));
                } 
                else if (msg == "!quote") 
                        botReply = "Talk is cheap,show me the code";
                else if(msg == "!joke")
                    botReply = "why programmer hate nature? too many bigs";
                else if(msg == "!advice")
                        botReply = "advice";
                else if(msg == "!help")
                    botReply = "Commands: !hello !time !quote !joke";
                else
                    botReply = "I only understand !hello !time !quote !joke !advice";

                std::string botMsg = ":" + bot->getNick() + "!" + bot->getName() + "@server PRIVMSG " + target + " :" + botReply + "\r\n";

                for (size_t j = 0; j < ch->getClients().size(); ++j)
                    if (ch->getClients()[j] != bot)
                        ch->getClients()[j]->sendMsg(botMsg);
            }
        }
        else
        {
            j = 0;
            while (j < clients.size())
            {
                if (clients[j]->getNick() == target)
                {
                    message = ":" + sender.getNick() + " PRIVMSG " + target + " :" + msg + "\r\n";
                    clients[j]->sendMsg(message);
                    found = true;
                    if (bot && target == bot->getNick() && !msg.empty() && msg[0] == '!')
                    {
                        std::string botReply;
                        if (msg == "!hello")
                            botReply = "Hello " + sender.getNick() + "! You called me.";
                        else if (msg == "!time") {
                            time_t now = time(0);
                            botReply = "Server time: " + std::string(ctime(&now));
                        } 
                        else if (msg == "!quote") 
                                botReply = "Talk is cheap,show me the code";
                        else if(msg == "!joke")
                            botReply = "why programmer hate nature? too many bigs";
                        else if(msg == "!advice")
                              botReply = "advice";
                        else if(msg == "!help")
                            botReply = "Commands: !hello !time !quote !joke";
                        else
                            botReply = "I only understand !hello !time !quote !joke !advice";

                        std::string botMsg = ":" + bot->getNick() + "!" + bot->getName() + "@server PRIVMSG " + sender.getNick() + " :" + botReply + "\r\n";
                        sender.sendMsg(botMsg);
                    }
                    break;
                }
                j++;
            }
            if (!found)
                sender.sendMsg(":irc.server 401 " + sender.getNick() + " " + target + " :No such nick\r\n");
        }
        i++;
    }
}

std::vector<std::string> token_mode(const std::string &line)
{
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;

    while (iss >> token)
        tokens.push_back(token);
    return tokens;
}
bool modeNeedArg(char mode)
{
    return (mode == 'k' || mode == 'l' || mode == 'o');
}

struct ModeChange
{
    char action;
    char mode;
    std::string arg;
};

std::vector<ModeChange> parseModetoens(const std::vector<std::string> &tokens)
{
    std::vector<ModeChange> changes;

    if (tokens.size() < 2)
        return changes;
    
    char curr = '+';
    size_t i = 1;

    std::string token;
    while (i < tokens.size())
    {
        token = tokens[i];
        bool consumedArg = false;
        if (token[0] == '+' || token[0] == '-')
        {
            curr = token[0];
            size_t j;
            for (j = 1; j < token.size(); ++j)
            {
                char mode = token[j];
                ModeChange change;
                change.action = curr;
                change.mode = mode;
                change.arg = "";

                if (modeNeedArg(mode))
                {
                    if (i + 1 < tokens.size())
                    {
                        ++i;
                        change.arg = tokens[i];
                        consumedArg = true;
                    }
                    else
                        change.arg = "";
                }
                changes.push_back(change);
            }

        }
        if (!consumedArg)
            ++i;
    }
    return changes;
}

void handleMode(Client &sender, const std::vector<std::string> &tokens, std::map<std::string, Channel*> &channels)
{
    std::string msg;
    if (tokens.empty())
    {
        msg = "not enough parametres\n";
        sender.sendMsg(msg);
        return;
    }
    std::string channelName = tokens[0];
    if (channels.find(channelName) == channels.end())
    {
        sender.sendMsg(":irc.server 403 " + sender.getNick() + " " + channelName + " :No such channel\r\n");
        return ;
    }
    Channel *ch = channels[channelName];
    if (!ch->isOperator(sender.getNick()))
    {
        sender.sendMsg(":irc.server 482 " + sender.getNick() + " " + channelName + " :You're not a channel operator\r\n");
        return;
    }
    std::vector<ModeChange> changes = parseModetoens(tokens);
    size_t i = 0;
    for (i = 0; i < changes.size(); ++i)
    {
        ModeChange &change = changes[i];
        if (change.mode == 'i')
        {
            ch->setInviteOnly(change.action == '+');
        }
        else if (change.mode == 't')
            ch->setTopicRes(change.action == '+');
        else if (change.mode == 'k')
        {
            if (change.action == '+')
                ch->setKey(change.arg);
            else
                ch->removeKey();
        }
        else if (change.mode == 'l')
        {
            if (change.action == '+')
                ch->setLimit(std::atoi(change.arg.c_str()));
            else
                ch->removeLimit();
        }
        else if (change.mode == 'o')
        {
            if (change.action == '+')
                ch->addOperator(change.arg);
            else
                ch->removeOperator(change.arg);
        }
        
        msg = ":" + sender.getNick() + " MODE " + channelName + " " + change.action + change.mode;
        if (!change.arg.empty())
            msg += " " + change.arg;
        msg += "\r\n";

        const std::vector<Client*> &clients = ch->getClients();
        for (size_t j = 0; j < clients.size(); ++j)
        {
                clients[j]->sendMsg(msg);
        }
    }
}

std::pair<std::string, std::string> parseTopicLine(const std::string &line)
{
    std::stringstream iss(line);
    std::string name;
    iss >> name;

    std::string rest;
    std::getline(iss, rest);
    if (!rest.empty() && rest[0] == ' ')
        rest.erase(0, 1);

    return std::make_pair(name, rest);
}

void handleTopci(Client &client, const std::string &line, std::map<std::string, Channel*> &channels)
{
    std::pair<std::string, std::string> parse = parseTopicLine(line);
    std::string name = parse.first;
    std::string text = parse.second;

    if (line.empty())
    {
        client.sendMsg(":irc.server 403 " + client.getNick() + ": no parameters\r\n");
        return ;
    } 
    if (channels.find(name) == channels.end())
    {
        client.sendMsg(":irc.server 403 " + client.getNick() + " " + name + " :No such channel\r\n");
        return ;
    }
    Channel *ch = channels[name];
    if (!ch->hasClient(&client))
    {
        client.sendMsg(":irc.server 442 " + client.getNick() + " " + name + " :You're not on that channel\r\n");
        return ;
    }
    if (text.empty())
    {
        client.sendMsg(":irc.server 332 " + client.getNick() + " " + name + " :" + ch->getTopic() + "\r\n");
    }
    else
    {
        if (ch->isTopicRes() && !ch->isOperator(client.getNick()))
        {
            client.sendMsg(":irc.server 482 " + client.getNick() + " " + name + " :You're not a channel operator\r\n");
            return;
        }
        ch->setTopic(text);

        std::string topicMsg = ":" + client.getNick() + " TOPIC " + name + " :" + text + "\r\n";
        
        const std::vector<Client*>& clients = ch->getClients();
        
        for (size_t i = 0; i < clients.size(); ++i)
        {
                clients[i]->sendMsg(topicMsg);
        }
    }
}

std::vector<std::string> parseKickLine(const std::string &line)
{
    std::vector<std::string> tokens;
    std::stringstream iss(line);
    std::string token;

    while (iss >> token)
    {
        if (token[0] == ':')
        {
            std::string rest;
            std::getline(iss, rest);
            token += rest;
            tokens.push_back(token);
            break;
        }
        tokens.push_back(token);
    }
    return tokens;
}

void handleKick(Client &client, std::vector<std::string> &tokens, std::map<std::string, Channel*> &channels, Client *bot)
{
    if (tokens.size() < 2)
    {
        client.sendMsg(":irc.server 461 " + client.getNick() + " :Not enough parameters\r\n");
        return ;
    }
    std::string name = tokens[0];
    std::string target = tokens[1];
    std::string text;

    if (tokens.size() > 2)
        text = tokens[2];
    else
        text = target;

    if (channels.find(name) == channels.end())
    {
        client.sendMsg(":irc.server 403 " + client.getNick() + " " + name + " :No such channel\r\n");
        return ;
    }
    Channel *ch = channels[name];
    if (!ch->isOperator(client.getNick()))
    {
        client.sendMsg(":irc.server 482 " + client.getNick() + " " + name + " :You're not a channel operator\r\n");
        return ;
    }

    Client *targetClient = NULL;
    const std::vector<Client*> &clients = ch->getClients();
    for (size_t i = 0; i < clients.size(); ++i)
    {
        if (clients[i]->getNick() == target)
        {
            targetClient = clients[i];
            break ;
        }
    }
    if (!targetClient)
    {
        client.sendMsg(":irc.server 441 " + client.getNick() + " " + name + " " + name + " :They aren't on that channel\r\n");
        return;
    }
    if(targetClient == bot){
        std::string msg = "you cant kick" + target + "\r\n";
        client.sendMsg(msg);
        return;
    }
    std::string msg = ":" + client.getNick() + " KICK " + name + " " + target + " :" + text + "\r\n";
    for (size_t i = 0; i < clients.size(); ++i)
        clients[i]->sendMsg(msg);
    ch->removeClient(targetClient);
    targetClient->removeChannel(name);

}

void handleInvite(Client &client, std::vector<std::string> &tokens, std::map<std::string, Channel*> &channels, std::vector<Client *> clients)
{
    if (tokens.size() < 2)
    {
        client.sendMsg(":irc.server 461 INVITE :Not enough parameters\r\n");
        return;
    }

    std::string target = tokens[0];
    std::string name = tokens[1];

    if (channels.find(name) == channels.end())
    {
        client.sendMsg(":irc.server 403 " + client.getNick() + " " + name + " :No such channel\r\n");
        return ;
    }
    Channel *ch = channels[name];
    
    if (!ch->hasClient(&client))
    {
        client.sendMsg(":irc.server 442 " + client.getNick() + " " + name + " :You're not on that channel\r\n");
        return;
    }

    Client *targetClient = NULL;
    for (size_t i = 0; i < clients.size(); ++i)
    {
        if (clients[i]->getNick() == target)
        {
            targetClient = clients[i];
            break ;
        }
    }
    if (!targetClient)
    {
        client.sendMsg(":irc.server 441 " + client.getNick() + " " + name + " " + name + " :They aren't on that channel\r\n");
        return;
    }

    ch->addInvite(targetClient->getNick());
    client.sendMsg(":irc.server 341 " + client.getNick() + " " + target + " " + name + "\r\n");
    targetClient->sendMsg(":" + client.getNick() + " INVITE " + target + " :" + name + "\r\n");
}

std::string extractInviteString(const std::string line)
{
    std::string joinLine = trim(line);
    if (joinLine.size() <= 6)
        return "";
    joinLine = joinLine.substr(6, joinLine.size());
    return (trim(joinLine));
}

void handleCommand(Client &client, const std::string &line, const std::string &serverPass, const std::vector<Client*> &clients, std::map<std::string, Channel*> &channels, Client *bot)
{
    std::string command = extractCommand(line);
    std::string text = extractString(line);

    if (command == "PASS")
        handlePass(client, text, serverPass);
    else if (command == "NICK" && client.isAuthenticated())
        handleNick(client, text, clients);
    else if (command == "USER" && client.isAuthenticated())
        handleUser(client, text);
    else if (client.isRegistred())
    {
        if (command == "JOIN")
        {
            std::string joinLine = extractJoinString(line);
            handleJoin(client, joinLine, channels, bot);
        }
        else if (command == "PRIVMSG")
        {
            handlePrivmsg(client, line, clients, channels, bot);
        }
        else if (command == "MODE")
        {
            std::string modeLine = extractJoinString(line);
            std::vector<std::string> tokens = token_mode(modeLine);
            handleMode(client, tokens, channels);
        }
        else if (command == "TOPIC")
        {
            std::string topicLine = extractJoinString(line);
            handleTopci(client, topicLine, channels);
        }
        else if (command == "KICK")
        {
            std::string kickLine = extractJoinString(line);
            std::vector<std::string> tokens = parseKickLine(kickLine);
            handleKick(client, tokens, channels, bot);
        }
        else if (command == "INVITE")
        {
            std::string inviteLine = extractInviteString(line);
            std::vector<std::string> tokens = token_mode(inviteLine);
            handleInvite(client, tokens, channels, clients);
        }
        else
        {
            std::string msg = "unknown command\r\n";
            client.sendMsg(msg);
        }
    }
    else
    {
        if(client.isAuthenticated())
        {
            std::string msg = "unknown command: for registration nick name and user name are mendatroy\r\n";
            client.sendMsg(msg);
        }
        else
        {
            std::string msg = "unknown command: for registration enter PASS\r\n";
            client.sendMsg(msg);
        }

    }
}
