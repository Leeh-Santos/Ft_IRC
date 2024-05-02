# include "Client.hpp"

# include "Server.hpp"

void Client::add_buffer(std::string str){
    (void)str;
}

int Client::is_verified() {return verified;}

void Client::set_verified(int x) {verified = x;}

void Client::set_nick(std::string str) {nick = str;}

std::string Client::get_nick() {return nick;}

void Client::set_user(std::string str) {user = str;}

std::string Client::get_user() {return user;}

void Client::set_bool_pass(int x) {pass = 1;}

int Client::get_bool_pass() {return pass;}