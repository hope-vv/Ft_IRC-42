#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include "Client.hpp"
#include <algorithm>
#include "Commands.hpp"

#define MAX_CLIENTS 100

class IRCServer {
private:
    std::string password;   
    int port;
    int nfds;
    int server_fd;
    bool running;
    
    struct sockaddr_in address;
    std::vector<Client *> clients;
    std::map<std::string, Channel*> channels;
    struct pollfd fds[MAX_CLIENTS];

    std::map<int, std::string> clientBuffers;
    std::map<int, std::string> sendBuffers;

    Client* getClientByFd(int fd);
    Client *bot;

public:
std::map<std::string, Channel*>& getChannels() { return channels; }
    IRCServer(const std::string &pass, int p);
    IRCServer();
    ~IRCServer();
    bool setupServer();
    void run();
    std::string CheckCommand(int fd);
    void ExecuteCommand(int fd);
    void createBot() ;
    void botJoinNewChannel(Channel *ch);
    void stop();
private:
    void acceptClient();
    void handleClient(int index);
};

#endif
