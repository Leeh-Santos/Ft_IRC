//#pragma once
#ifndef SERVER_HPP 
#define SERVER_HPP 

# include "Client.hpp"

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
//-------------------------------------------------------//
#define RED "\e[1;31m" 
#define WHI "\e[0;37m" 
#define GRE "\e[1;32m" 
#define YEL "\e[1;33m" 

class Client;

class Server 
{
private:
	int Port; //-> server port
	int SerSocketFd; //-> server fd
	static bool Signal; 
	std::vector<Client> clients; 
	std::vector<struct pollfd> fds; 
	
	std::string serverpass;
public:
	Server(int port, char *pass){
		SerSocketFd = -1;
		Port = port;
		std::string tmp(pass);
		serverpass = tmp;
	} 
	void ServerInit(); 
	void SerSocket(); 
	void AcceptNewClient(); 
	void ReceiveNewData(int fd, Client&); 
	static void SignalHandler(int signum); 
	void CloseFds(); 
	void ClearClients(int fd); 

	Client& get_client(int, std::vector<Client>);
};


# endif