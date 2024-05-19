#include "Channel.hpp"

Channel::Channel() {
	std::cout << "Channel stadard constructor called" << std::endl;}

Channel::Channel(std::string name){
	_channelName = name;
	_keyChannelModeAndValue = "";
	_topic = "";
	_topicMode = 0;
	_inviteOnlyChannelMode = 0;
	_clientLimitChannelModeAndValue = 0;
	std::cout << "Channel created with id: " << name << std::endl;
	//Server::addChannel(name);
}

Channel::~Channel() {
	std::cout << "Channel " << _channelName << " destructer called" << std::endl;}

void Channel::addClient(const Client& client) {
	_clientsList.push_back(client);
}

//SETTERS E GETTERS

std::string			Channel::getChannelName() const {return _channelName;}

std::vector<Client> Channel::getClientsList() const {return _clientsList;}

std::vector<Client> Channel::getInvitedClientsList() const {return _invitedClientsList;}

std::string			Channel::getOperator() const {return _operator;}

bool				Channel::getInviteOnlyChannelMode() const {return _inviteOnlyChannelMode;}

bool				Channel::getTopicMode() const {return _topicMode;}

std::string			Channel::getTopic() const {return _topic;}

std::string			Channel::getkeyChannelModeAndValue() const {return _keyChannelModeAndValue;}

int					Channel::getClientLimitChannelModeAndValue() const {return _clientLimitChannelModeAndValue;}

