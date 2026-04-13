#include "../include/Channel.hpp"

Channel::Channel(const std::string &name) : name(name), key(""), hasKey(false), inviteOnly(false), topicRes(false), userLimit(0)
{
}

const std::string &Channel::getName() const
{
    return (this->name);
}


const std::string &Channel::getKey() const
{
    return (this->key);
}

const std::vector<Client*> &Channel::getClients() const
{
    return (this->Clients);
}

void Channel::addClient(Client *client)
{
    if (!hasClient(client))
        this->Clients.push_back(client);
}


bool Channel::hasClient(Client *client) const
{
    std::vector<Client*>::const_iterator it = Clients.begin();
    while (it != Clients.end())
    {
        if (*it == client)
            return true;
        ++it;
    }
    return (false);
}



////// MODES
void Channel::setInviteOnly(bool val){ inviteOnly = val;}

bool Channel::isInviteOnly() const { return inviteOnly; }

void Channel::setTopicRes(bool val) { topicRes = val; }

bool Channel::isTopicRes() const { return topicRes; }

void Channel::setKey(const std::string &key_)
{
    this->key = key_;
    hasKey = true;
}

void Channel::removeKey()
{
    this->key.clear();
    hasKey = false;
}

bool Channel::isKeySet() const { return hasKey; }

void Channel::setLimit(size_t limit) { this->userLimit = limit; }

void Channel::removeLimit() { this->userLimit = 0; }

size_t Channel::getLimit() const { return this->userLimit; }

// bool Channel::hasLimit() const { return this->userLimit > 0; }


void Channel::addOperator(const std::string &nick)
{
    size_t i = 0;
    while (i < this->operators.size()){
        if (this->operators[i] == nick)
            return;
        i++;
    }
    this->operators.push_back(nick);
}

void Channel::removeOperator(const std::string &nick)
{
    size_t i = 0;
    while (i < this->operators.size()){
        if (this->operators[i] == nick)
        {
            this->operators.erase(this->operators.begin() + i);
            return;
        }
        i++;
    }
}

bool Channel::isOperator(const std::string &nick) const
{
    size_t i = 0;
    while (i < this->operators.size()){
        if (this->operators[i] == nick)
            return true;
        i++;
    }
    return false;
}


bool Channel::isInvited(const std::string &nick) const
{
    for (size_t i = 0; i < invited.size(); ++i) {
        if (invited[i] == nick)
            return true;
    }
    return false;
}

const std::string &Channel::getTopic() const { return topic; }

void Channel::setTopic(const std::string &t) { topic = t; }

void Channel::removeClient(Client *client)
{
    for (size_t i = 0; i < this->Clients.size(); ++i)
    {
        if (this->Clients[i] == client)
        {
            this->Clients.erase(this->Clients.begin() + i);
            break;
        }
    }
    for (size_t i = 0; i < this->invited.size(); ++i)
    {
        if (this->invited[i] == client->getNick())
        {
            this->invited.erase(this->invited.begin() + i);
            break;
        }
    }
}

void Channel::addInvite(const std::string &nick)
{
    for (size_t i = 0; i < this->invited.size(); ++i)
    {
        if (this->invited[i] == nick)
            return ;
    }
    this->invited.push_back(nick);
}
