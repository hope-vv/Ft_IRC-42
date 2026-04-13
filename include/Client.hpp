#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <string>
#include <vector>
#include <algorithm>



class Client
{
    private:
    bool authenticated;
    std::string nickname;
    std::string username;
    bool isconnected;

    std::vector<std::string> joinedChannels;
    
    
    public:
        int fd;
        int password;
        Client(int fd_);
        ~Client();

        int getFd() const;
        //PASS
        bool isAuthenticated() const;
        void setAuthenticated(bool val);
        bool isConnected() const;
        void setConnected(bool val);
        //NICK NAME
        void setNick(std::string name);
        std::string getNick() const;
        bool isRegistred() const;
        //user name
        void setName(const std::string &user_name);
        std::string getName() const;

        void send_welcome(Client &client);

        void sendMsg(const std::string &msg) const;

        //channels joined
        void addChannel(const std::string &name);
        const std::vector<std::string>& getChannels() const;

        void removeChannel(const std::string &name);

};


#endif
