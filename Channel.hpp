#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Server.hpp"
# include <string>
# include <vector>

class Client;

class Channel {

private:
	std::string				_channelName;
	std::vector<Client>		_clientsList;
	std::vector<Client>		_invitedClientsList;
	std::string				_operator;
	//MODES
	bool					_inviteOnlyChannelMode;	//"+i" controls whether new users need to be invited to the channel before being able to join
	bool					_topicMode;				//"+t" controls whether channel privileges are required to set the topic
	std::string				_topic;
	std::string				_keyChannelModeAndValue;		//"+k" sets a ‘key’ that must be supplied in order to join this channel
	int						_clientLimitChannelModeAndValue;//"+l" controls whether new users may join based on the number of users who already exist

public:
	Channel();
	Channel(std::string name);
	~Channel();

	void addClient(const Client& client);

	//SETTERS E GETTERS
	std::string 		getChannelName() const;
	std::vector<Client> getClientsList() const;
	std::vector<Client> getInvitedClientsList() const;
	std::string			getOperator() const;
	bool				getInviteOnlyChannelMode() const;
	bool				getTopicMode() const;
	std::string			getTopic() const;
	std::string			getkeyChannelModeAndValue() const;
	int					getClientLimitChannelModeAndValue() const; // if = 0 mode off, if > 0 is value of the limit
};

#endif