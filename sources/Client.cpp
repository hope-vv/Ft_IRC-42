#include "../include/Client.hpp"

Client::Client(int fd_) : authenticated(false), nickname(""), username(""), isconnected(false), fd(fd_)
{}

Client::~Client()
{

}

int Client::getFd() const
{
    return (this->fd);
}

bool Client::isAuthenticated() const
{
    return (this->authenticated);
}

bool Client::isConnected() const
{
    return (this->isconnected);
}

void Client::setConnected(bool val)
{
    this->isconnected = val;
}

void Client::setAuthenticated(bool val)
{
    this->authenticated = val;
}

void Client::setNick(std::string name)
{
    this->nickname = name;
}
std::string Client::getNick() const
{
    return (this->nickname);
}
bool Client::isRegistred() const 
{
    std::string msg;
    return (!this->getNick().empty() && !this->getName().empty() && this->authenticated);
}
void Client::setName(const std::string &user_name)
{
    this->username = user_name;
}

std::string Client::getName() const
{
    return (this->username);
}

void Client::send_welcome(Client &client)
{
    int fd = client.getFd();
    std::string nick = client.getNick();

    std::string msg = ":irc.server " + nick +
        " :Welcome to the IRC network, " + nick + "\r\n";
    send(fd, msg.c_str(), msg.size(), 0);

}

void Client::sendMsg(const std::string &msg) const
{
    if (fd == -1) { 
        return;
    }

    if (send(fd, msg.c_str(), msg.length(), 0) == -1)
        std::cerr << "Error sending message to client fd " << fd << std::endl;
}

void Client::addChannel(const std::string &name)
{
    if (std::find(joinedChannels.begin(), joinedChannels.end(), name) == joinedChannels.end())
        joinedChannels.push_back(name);
}

const std::vector<std::string>& Client::getChannels() const
{
    return joinedChannels;
}

void Client::removeChannel(const std::string &name)
{
    for (size_t i = 0; i < this->joinedChannels.size(); ++i)
    {
        if (this->joinedChannels[i] == name)
        {
            this->joinedChannels.erase(joinedChannels.begin() + i);
            break;
        }
    }
}
