# include "Server.hpp"
# include "Client.hpp"

void Server::cmd_execute(std::string cli_str, Client& cli) {
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

void	Server::joinChannel(Client& cli, std::string channelName) {
 channels->add
}

void	Server::checkPassJoinOrReturn(std::vector<Channel>::iterator it, Client& cli, std::string channelName, std::string pass)
{
	if (!it->getchannelPass().empty()) {		//Se ha pass
		if (pass == it->getchannelPass())			//E se pass correcta
			joinChannel(cli, channelName);
		else {											//Se pass e esta incorrecta
			sendIrcMessage(cli.GetFd(), ":@localhost 475 " + cli.get_nick() + " " + channelName + " :Bad channel password");
			return ;
		}
	}
	else										//Se nao ha pass
		joinChannel(cli, channelName);
}

void	Server::join_cmd(std::string cmd_line, Client& cli) { //watch out with /r/n  join #asdasdas /r/n
	std::vector<std::string> cmd = tokenit_please(cmd_line, 1);

	if (cmd.size() < 2) {
		sendIrcMessage(cli.GetFd(), ":@localhost 461 " + cli.get_nick() + " JOIN :Not enough parameters\n");
		return;
	}

	std::string channelName = cmd[1];
	std::string  pass = cmd[2];

	if (channelName[0] != '#') { // Se não começar por #, primeiro char
		sendIrcMessage(cli.GetFd(), ":@localhost 403" + cli.get_nick() + " " + cmd[1] + " No such channel\n");
		return;
	}

	std::vector<Channel>::iterator it = std::find(channels.begin(), channels.end(), channelName);
	// Se esse canal tiver limiteMode ligado (limite de quantidade de clients no canal) e superior
	if (it->getClientLimitChannelModeAndValue() && it->getClientsList().size() >= it->getClientLimitChannelModeAndValue()) {
		sendIrcMessage(cli.GetFd(), ":@localhost 471 " + cli.get_nick() + " " + channelName + " :Cannot join channel (+l)");
		return ;
	}

	if (it != channels.end()) {			//IF CHANNEL EXIST
		if (it->getInviteOnlyChannelMode()) {	//ON, invite mode
			std::vector<Client>::iterator itInvite = std::find(it->getInvCliList().begin(), it->getInvCliList().end(), cli.get_nick());
			if (itInvite != it->getInvCliList().end())	//Se client esta na lista
				checkPassJoinOrReturn(it, cli, channelName, pass);
			else										//Se nao esta na lista
				sendIrcMessage(cli.GetFd(), ":@localhost 473 " + cli.get_nick() + " " + channelName + " :Invite only channel");
		}
		else 									//OFF invite mode
			checkPassJoinOrReturn(it, cli, channelName, pass);
	}
	else {								//IF CHANNEL DOES NOT EXIST, create it
		this->channels.push_back(Channel(channelName));
		sendChannelMessage( ":" + cli->getNick() + "!" + cli.get_user() + "@localhost" + " JOIN " + channelName);
		sendIrcMessage(":@localhost 332 " + cli->getNick() + " " + channelName + " :No topic is set", cli->get_user());
	}
}
