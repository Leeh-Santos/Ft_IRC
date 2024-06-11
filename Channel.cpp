#include "Channel.hpp"

Channel::Channel() {
	std::cout << "Channel stadard constructor called" << std::endl;}

Channel::Channel(std::string name){  //default mode: +t -i -k -l
	_channelName = name;
	_channelPass = "";
	_topic = "";
	_topicMode = 0;
	_inviteOnlyChannelMode = 0;
	_clientLimitChannelModeAndValue = 0;
	_operator = "";
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

void				Channel::setOperator(std::string op){
	_operator = op;
}

void 	Channel::removeClient(std::string target){
	std::vector<Client>::iterator it;
	for (it = _InvCliList.begin(); it != _InvCliList.end(); ++it){
		if ((it)->get_nick() == target){
			_clientsList.erase(it);
			break;
		}
	}
	
	std::vector<Client>::iterator it2;
	for (it2 = _clientsList.begin(); it2 != _clientsList.end(); ++it2){
		if ((it2)->get_nick() == target){
			_clientsList.erase(it2);
			break;
		}
	}
}