//#pragma once
#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "Server.hpp"
# include <string>
# include <vector>

class Server;

class Client
{
private:
	int Fd; //-> client file descriptor
	std::string IPadd; //-> client ip address

	std::string _buffer;
	bool _pass;
	std::string _nick;
	std::string _user;
	bool _verified;

public:
	Client(){}; //-> default constructor
	int GetFd(){return Fd;} //-> getter for fd
	void SetFd(int fd){Fd = fd;} //-> setter for fd
	void setIpAdd(std::string ipadd){IPadd = ipadd;} //-> setter for ipadd

	void set_verified(int x);
	void set_buffer(std::string str);
	std::string get_buffer();
	int is_verified();

	void set_nick(std::string str);
	std::string get_nick();
	void set_user(std::string str);
	std::string get_user();
	void set_bool_pass(int x);
	int get_bool_pass();
};

#endif