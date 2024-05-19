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
	else if (first_word == "join" || first_word == "JOIN")
		join_cmd(cli_str, cli);
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

//JOIN COMMAND

void	Server::joinChannel(Client& cli,std::string channelName) {
	//JoinChannel
}

void	Server::join_cmd(std::string cmd_line, Client& cli) { //watch out with /r/n  join #asdasdas /r/n
	std::vector<std::string> cmd = tokenit_please(cmd_line, 1);
	if (cmd.size() < 2) {
		sendIrcMessage(cli.GetFd(), ":@localhost 461 " + cli.get_nick() + " JOIN :Not enough parameters\n");
		return;
	}
		std::string channelName = cmd[1];
	//if (!cmd[2].empty())
		std::string passUser = cmd[2];
	if (channelName[0] != '#') { //se não começar por #, primeiro char
		sendIrcMessage(cli.GetFd(), ":@localhost 403" + cli.get_nick() + " " + cmd[1] + " No such channel\n");
		return;
	}
	std::vector<Channel>::iterator it = std::find(channels.begin(), channels.end(), channelName); //Falta considerar se pass estiver desligado
	if (it != channels.end()) { 	//channel exist
		if (it->getInviteOnlyChannelMode() && !it->getkeyChannelModeAndValue().empty()) { //se invite mode ON e se tem password
			std::vector<Client>::iterator itInvite = std::find(it->getInvitedClientsList().begin(), it->getInvitedClientsList().end(), cli.get_nick());
			if (itInvite != it->getInvitedClientsList().end()) { //Se esta na lista
				if (passUser == it->getkeyChannelModeAndValue()) //E se pass correcta
					joinChannel(cli, channelName);
				else { //se pass incorrecta
					sendIrcMessage(cli.GetFd(), ":@localhost 475 " + cli.get_nick() + " " + channelName + " :Bad channel password");
					return ;
				}
			}
			else { // Se nao esta na lista
				sendIrcMessage(cli.GetFd(), ":@localhost 473 " + cli.get_nick() + " " + channelName + " :Invite only channel");
			}
		}
		//se esse canal tiver limiteMode ligado (limite de quantidade de clients no canal) e superior
		if (it->getClientLimitChannelModeAndValue() && it->getClientsList().size() >= it->getClientLimitChannelModeAndValue()) {
			sendIrcMessage(cli.GetFd(), ":@localhost 471 " + cli.get_nick() + " " + channelName + " :Cannot join channel (+l)");
			return ;
		}
	}

	else {	//channel does not exist

	}




}