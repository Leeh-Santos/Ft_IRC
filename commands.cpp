# include "Server.hpp"
# include "Client.hpp"

void Server::cmd_execute(std::string cli_str, Client& cli){
	std::string first_word = cli_str.substr(0, cli_str.find(' '));

	if (first_word == "pass" || first_word == "PASS")
		client_sender(cli.GetFd(), ":Server 462 Already registered");
	else if (first_word == "user" || first_word == "USER")
		client_sender(cli.GetFd(), ":Server 462 Already registered");
	else if (first_word == "nick" || first_word == "NICK")
		change_nick(cli_str, cli);
	else
		std::cout << "first word: " << first_word << "/ string inteira :" << cli_str << std::endl;

	
}

void Server::change_nick(std::string cli_str, Client& cli){
	std::string nick = cli_str.substr(cli_str.find_first_not_of("nick "));
	nick = str_cutter(nick); // limpar a string
	if (verify_nicks(nick)){
		client_sender(cli.GetFd(), ":Server 433 Nick " + nick + " is already in use");
		return;
	}
	if(nick[0] == '#' || nick[0] == ':'){
		client_sender(cli.GetFd(), ":Server 432 Erroneus nickname");
		return;
	}
	if(nick.empty())
		return;
	if(nick.size() > 10)
		nick = nick.substr(0, 9);
	cli.set_nick(nick);
	client_sender(cli.GetFd(), ":Server new NICK succesfully added ->" + cli.get_nick());
}

std::string Server::str_cutter(std::string str){
	if (str.find("\r\n") != std::string::npos)
		str = str.substr(0, str.find("\r\n"));
	else
		str = str.substr(0, str.find("\n"));
	return str;
}   