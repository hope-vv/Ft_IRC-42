#ifndef CHANNEL_HPP
#define CHANNEL_HPP


#include <string>
#include <vector>
#include "Client.hpp"

class Channel
{
    private:
        std::string name;
        std::string key;
        std::vector<Client*> Clients;

        std::vector<std::string> operators;
        std::vector<std::string> invited;
        std::string topic;

        bool hasKey;
        bool inviteOnly;
        bool topicRes;
        size_t userLimit;


    public:
        Channel(const std::string &name);

        const std::string &getName() const;
        const std::string &getKey() const;
        const std::vector<Client*> &getClients() const;

        void addClient(Client *client);
        bool hasClient(Client *client) const;

        //mode handling
        void setInviteOnly(bool val);
        bool isInviteOnly() const;

        void setTopicRes(bool val);
        bool isTopicRes() const;

        void setKey(const std::string &key_);
        void removeKey();
        bool isKeySet() const;

        void setLimit(size_t limit);
        void removeLimit();
        size_t getLimit() const;
        // bool hasLimit() const;

        void addOperator(const std::string &nick);
        void removeOperator(const std::string &nick);
        bool isOperator(const std::string &nick) const;

        //invite
        bool isInvited(const std::string &nick) const;
        void addInvite(const std::string &nick);

        //topic
        const std::string &getTopic() const;
        void setTopic(const std::string &t);

        //kick
        void removeClient(Client *client);
        
};

#endif