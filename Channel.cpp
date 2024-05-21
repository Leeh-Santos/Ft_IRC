#include "Channel.hpp"

Channel::Channel() {
	std::cout << "Channel stadard constructor called" << std::endl;}

Channel::Channel(std::string name){
	_channelName = name;
	_channelPass = "";
	_topic = "";
	_topicMode = 0;
	_inviteOnlyChannelMode = 0;
	_clientLimitChannelModeAndValue = 0;
	std::cout << "Channel created with id: " << name << std::endl;
	//addChannel(name);
}

Channel::~Channel() {
	std::cout << "Channel " << _channelName << " destructer called" << std::endl;}

void Channel::addClient(const Client& client) {
	_clientsList.push_back(client);
}

//SETTERS E GETTERS

std::string			Channel::getChannelName() const {return _channelName;}

std::vector<Client> Channel::getClientsList() const {return _clientsList;}

std::vector<Client> Channel::getInvCliList() const {return _InvCliList;}

std::string			Channel::getOperator() const {return _operator;}

bool				Channel::getInviteOnlyChannelMode() const {return _inviteOnlyChannelMode;}

bool				Channel::getTopicMode() const {return _topicMode;}

std::string			Channel::getTopic() const {return _topic;}

std::string			Channel::getchannelPass() const {return _channelPass;}

int					Channel::getClientLimitChannelModeAndValue() const {return _clientLimitChannelModeAndValue;}

