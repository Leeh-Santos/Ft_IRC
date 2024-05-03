# include "Client.hpp"

# include "Server.hpp"

void Client::set_buffer(std::string str){ _buffer += str;} //talves += dps lets keeping adding shit to buffer till /r

std::string Client::get_buffer(){ return _buffer;}

int Client::is_verified() {return verified;}

void Client::set_verified(int x) {verified = x;}

void Client::set_nick(std::string str) {nick = str;}

std::string Client::get_nick() {return nick;}

void Client::set_user(std::string str) {user = str;}

std::string Client::get_user() {return user;}

void Client::set_bool_pass(int x) {pass = x;}

int Client::get_bool_pass() {return pass;}