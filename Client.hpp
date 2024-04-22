//#pragma once

#ifndef CLIENT_HPP
#define CLIENT_HPP
# include "Server.hpp"
# include <string>

class Client
{
private:
    int Fd; //-> client file descriptor
    std::string IPadd; //-> client ip address

    std::string buffer;
    std::string pass;
    std::string nick;
    std::string user;

public:
    Client(){}; //-> default constructor
    int GetFd(){return Fd;} //-> getter for fd

    void SetFd(int fd){Fd = fd;} //-> setter for fd
    void setIpAdd(std::string ipadd){IPadd = ipadd;} //-> setter for ipadd
};

# endif