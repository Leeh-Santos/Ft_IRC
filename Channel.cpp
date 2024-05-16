#include "Channel.hpp"

Channel::Channel() {}

Channel::~Channel() {}

Channel::Channel(std::string name){
	_channelName = name;
	_keyChannelMode = "";
	_topic = "";
	_topicMode = 0;
	_inviteOnlyChannelMode = 0;
	_clientLimitChannelMode = 0;
	std::cout << "Channel created with id: " << name << std::endl;
	Server::addChannel(name);
}