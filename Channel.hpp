#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "Client.hpp"

class Client;

class Channel {

private:
	std::string				_channelName;
	std::vector<Client*>	_clients;
	std::vector<Client*>	_invited;
	std::string				_operators;
	//MODES
	bool					_inviteOnlyChannelMode;	//"+i" controls whether new users need to be invited to the channel before being able to join
	bool					_topicMode;				//"+t" controls whether channel privileges are required to set the topic
	std::string				_topic;
	std::string				_keyChannelMode;		//"+k" sets a ‘key’ that must be supplied in order to join this channel
	bool					_clientLimitChannelMode;//"+l" controls whether new users may join based on the number of users who already exist
public:

	Channel();
	Channel(std::string name);
	~Channel();
};

#endif