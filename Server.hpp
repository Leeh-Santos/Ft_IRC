//#pragma once
#ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp"
# include "Channel.hpp"

#include <iostream>
#include <vector> //-> for vector
#include <sys/socket.h> //-> for socket()
#include <sys/types.h> //-> for socket()
#include <netinet/in.h> //-> for sockaddr_in
#include <fcntl.h> //-> for fcntl()
#include <unistd.h> //-> for close()
#include <arpa/inet.h> //-> for inet_ntoa()
#include <poll.h> //-> for poll()
#include <csignal> //-> for signal()
#include <stdio.h>
#include <string.h>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <sstream>
//-------------------------------------------------------//
#define RED "\e[1;31m"
#define WHI "\e[0;37m"
#define GRE "\e[1;32m"
#define YEL "\e[1;33m"

class Client;
class Channel;

class Server
{
private:
	int							_port; //-> server port
	int							_serSocketFd; //-> server fd
	static bool					_signal;
	std::vector<Client>			_clients;
	std::vector<struct pollfd>	_fds;
	std::vector<Channel>		_channels;
	std::string					_serverPass;
public:
	Server(int port, char *pass){
		_serSocketFd = -1;
		_port = port;
		std::string tmp(pass);
		_serverPass = tmp;
	}
	void		ServerInit();
	void		SerSocket();
	void		AcceptNewClient();
	void		ReceiveNewData(int fd, Client& cli);
	static void	SignalHandler(int signum);
	void		CloseFds();
	void		ClearClients(int fd);

	void client_sender(int fd, std::string str){
	str += "\r\n";
	if(send(fd, str.c_str(), str.size(), 0) == -1)
		std::cerr << "not able to send data" << std::endl;
	std::cout << str; //print in the server side to see what is going on
	}

	//COMMANDS
	void		cmd_execute(std::string cli_str, Client& cli);
	void		change_nick(std::string cli_str,Client& cli);
	//JOIN
	void		checkPassJoinOrReturn(std::vector<Channel>::iterator it, Client& cli, std::string channelName, std::string pass);
	void		joinChannel(std::vector<Channel>::iterator channel, Client& cli, std::string channelName);
	bool		channelNameEquals(const Channel& channel, const std::string& name);
	void		join_cmd(std::string cmd_line, Client& cli);

	std::string	str_cutter(std::string);

	//ServerUtils
	void		handle_cap(std::string str, Client& cli);
	void		handle_nc(std::string str, Client& cli);
	void		validate_cli(Client&);
	std::vector<std::string> tokenit_please(std::string str, int x);
	bool		verify_nicks(std::string str);
	void		sendMsgToClient(int clientFd, std::string message);
	void		sendlMsgToChannel(std::vector<Client> clientsList, std::string message);

	//GETTERS AND SETTERS
	Client& get_client(int fd, std::vector<Client>&);
	std::vector<Channel>& getChannels();
	//void Server::setChannels(const std::vector<Channel>& newChannels);
};
# endif