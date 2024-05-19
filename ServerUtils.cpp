# include "Server.hpp"
# include "Client.hpp"

void Server::validate_cli(Client& cli){

	if (cli.get_nick() == "" || cli.get_user() == "" || cli.get_bool_pass() == 0){
		client_sender(cli.GetFd(), "YOUR REGISTRATION STATUS SO FAR:");
		if (cli.get_nick() == "")
			client_sender(cli.GetFd(), "Nick is missing");
		if (cli.get_user() == "")
			client_sender(cli.GetFd(), "User is missing");
		if (cli.get_bool_pass() == 0)
			client_sender(cli.GetFd(), "Password is missing");
		client_sender(cli.GetFd(), "if you are using hexchat, please restart app with correct parameters Nick name/User name/Password");
	}
	else if (cli.get_nick() != "" && cli.get_user() != "" && cli.get_bool_pass() != 0) {
		cli.set_verified(1);
		client_sender(cli.GetFd(), "Welcome to IRC SERVER meu Parceiro");
	}
}

std::vector<std::string> Server::tokenit_please(std::string str, int x){
	std::vector<std::string> tokens;
	std::istringstream iss(str);
	std::string token;

	if (x){ //tokenize for /r/n and spaces
		for (size_t i = 0; i < str.length(); ++i) {
			if (str[i] == ' ' || str[i] == '\r' || str[i] == '\n') {
				if (!token.empty()) {
					tokens.push_back(token);
					token.clear();
				}
			} else {
				token += str[i];
			}
		}
		if (!token.empty()) {
			tokens.push_back(token);
		}
	}
	 else{ // for /n and spaces
		while (std::getline(iss, token, ' '))
			   tokens.push_back(token);
	}

	return tokens;
}


void Server::handle_cap(std::string str, Client& cli){

	std::vector<std::string> cap_tken_receiver = tokenit_please(str, 1);

	std::vector<std::string>::iterator it = std::find(cap_tken_receiver.begin(), cap_tken_receiver.end(), "PASS");
	if (it != cap_tken_receiver.end()){
		std::string pass_str = *++it;
		if(!pass_str.compare(serverpass)){
			if (cli.get_bool_pass()){
				client_sender(cli.GetFd(), ":Server 462 Already registered");
				return;
			}
			cli.set_bool_pass(1);
			client_sender(cli.GetFd(), ":Server Correct passoword!");
		} else
			client_sender(cli.GetFd(), ":Server 464 incorrect password!");  // (": 464 " + nickname + " :Password incorrect !" + CRLF )
	}
	std::vector<std::string>::iterator it1 = std::find(cap_tken_receiver.begin(), cap_tken_receiver.end(), "NICK");
	if (it1 != cap_tken_receiver.end()){
		std::string nicks = *++it1;
		if (verify_nicks(nicks)){
			client_sender(cli.GetFd(), ":Server 433 Nick " + str + " is already in use");
			return;
		}
		if(nicks[0] == '#' || nicks[0] == ':'){
			client_sender(cli.GetFd(), ":Server 432 Erroneus nickname");
			return;
		}
		if(nicks.empty())
			return;
		if(nicks.size() > 10)
			nicks = nicks.substr(0, 9);
		cli.set_nick(nicks);
		client_sender(cli.GetFd(), "NICK succesfully added ->" + cli.get_nick());
	}
	std::vector<std::string>::iterator it2 = std::find(cap_tken_receiver.begin(), cap_tken_receiver.end(), "USER");
	if (it2 != cap_tken_receiver.end()){
		cli.set_user(*++it2);
		client_sender(cli.GetFd(), "USER succesfully added ->" + cli.get_user());
	}

	/*		std::vector<std::string>::iterator i = cap_tken_receiver.begin();
	std::cout << "size of tokens : " << cap_tken_receiver.size() << std::endl;
	std::cout << "tokens : ";
	while (i != cap_tken_receiver.end()){
		std::cout << *i << " ";
		i++;
	}*/

}
void Server::handle_nc(std::string str, Client& cli){
		if (str.find("pass ") != std::string::npos){
			str = str.substr(str.find_first_not_of("pass "));
			str = str.substr(0, str.find('\n')); // nc sends /n nesse caralho
			if (cli.get_bool_pass()){
				client_sender(cli.GetFd(), ":Server 462 Already registered");
				return;
			}
			else if (!str.compare(serverpass)){
				cli.set_bool_pass(1);
				client_sender(cli.GetFd(), ":Server correct password!");
			}
			else{
				client_sender(cli.GetFd(), ":Server 464 Wrong password"); //(": 464 " + nickname + " :Password incorrect !" + CRLF )
			}
	}
	else if (str.find("nick ") != std::string::npos){
		change_nick(str, cli);
	}
	else if (str.find("user ") != std::string::npos){
		str = str.substr(str.find_first_not_of("user "));
		str = str.substr(0, str.find('\n'));
		if(str.empty())
			return;
		cli.set_user(str);
		client_sender(cli.GetFd(), ":Server User setted succesfully to :" + str);
	}

	/*std::cout << "CLIENT INFO " << std::endl;
	std::cout << "CLIENT user :" << cli.get_user() <<std::endl;
	std::cout << "CLIENT nick :" << cli.get_nick() <<std::endl;
	std::cout << "CLIENT pass :" << cli.get_bool_pass() <<std::endl;*/
}

bool Server::verify_nicks(std::string str){
	for(unsigned int i = 0; i < clients.size(); i++){
		if (!str.compare(clients[i].get_nick()))
			return 1;
	}
	return 0;
}

int sendIrcMessage(int clientFd, std::string message) {
	message = message + "\r\n";
	std::cout << "Sendind to client ID, " << clientFd << ": " << message << std::endl;
	if (send(clientFd, message.c_str(), message.length(), 0) < 0) {
		std::cout << "Error sending message to client" << std::endl;
		exit(1);
	}
	message.clear();
	return (0);
}

//SETTERS AND GETTERS
std::vector<Channel>& Server::getChannels() {return channels;}

//void Server::setChannels(const std::vector<Channel>& newChannels) {channels = newChannels;}