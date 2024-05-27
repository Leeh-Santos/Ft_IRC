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

void	Server::joinChannel(int i, Client& cli, std::string channelName, int adm_flag) {
	_channels[i].addClient(cli);
	sendMsgToClient(cli.GetFd(), ":@localhost 332 " + cli.get_nick() + " " + channelName + " :" + _channels[i].getTopic() + "\r\n");
	sendlMsgToChannel(_channels[i].getClientsList(), ":" + cli.get_nick() + "!" + cli.get_user() + "@localhost" + " JOIN " + channelName  + "\r\n");
	if (adm_flag){
		_channels[i].setOperator(cli.get_nick());
		//sendlMsgToChannel(_channels[i].getClientsList(), ":" + cli.get_nick() + "!" + cli.get_user() + "@localhost" + " JOIN " + channelName  + "\r\n");
		//sendMsgToClient(cli.GetFd(), ":@localhost 324 " + cli.get_nick() + " " + channelName + " +" + _channels[i].getOperator() + "\r\n"); //genereted by autopilot
	}
	//else BROAD CAST ou manda outra mensagem ou ja ta?

}

void	Server::checkPassJoinOrReturn(int i, Client& cli, std::string channelName, std::string pass)
{
	if (_channels[i].getchannelPass().empty()) {		//Se ha pass
		if (pass == _channels[i].getchannelPass())			//E se pass correcta
			joinChannel(i, cli, channelName, 0);
		else {											//Se pass e esta incorrecta
			sendMsgToClient(cli.GetFd(), ":@localhost 475 " + cli.get_nick() + " " + channelName + " :Bad channel password");
			return ;
		}
	}
	else										//Se nao ha pass
		joinChannel(i, cli, channelName, 0);
}

void	Server::join_cmd(std::string cmd_line, Client& cli) { //watch out with /r/n  join #asdasdas /r/n

	std::vector<std::string> cmd = tokenit_please(cmd_line, 1);
	std::string channelName = cmd[1];
	std::string  pass = cmd[2];

	if (!verify_channelName(channelName, cmd, cli))
		return;
	else if(channel_exists(channelName) == -1){ // se nao exite criamos, -1 nao temos canal
		this->_channels.push_back(Channel(channelName));
		Channel& newChannel = _channels.back();
		sendlMsgToChannel(newChannel.getClientsList(), ":" + cli.get_nick() + "!" + cli.get_user() + "@localhost" + " JOIN " + channelName + "\r\n");
		sendMsgToClient(cli.GetFd(), ":@localhost 332 " + cli.get_nick() + " " + channelName + " :No topic is set" + "\r\n");
		joinChannel(_channels.size() - 1, cli, channelName, 1);

	}else {
		unsigned int i = channel_exists(channelName); //se tiver o canal channel_exists() devolve o index
		if (_channels[i].getClientLimitChannelModeAndValue() && _channels[i].getClientsList().size() >= (size_t)_channels[i].getClientLimitChannelModeAndValue()) {
			sendMsgToClient(cli.GetFd(), ":@localhost 471 " + cli.get_nick() + " " + channelName + " :Cannot join channel (+l)");
			return ;
		}
		//
		if (_channels[i].getInviteOnlyChannelMode()) {	//ON, invite mode
			if (check_invite_list(i, cli)) // se tiver na list	
				checkPassJoinOrReturn(i, cli, channelName, pass);
			else										
				sendMsgToClient(cli.GetFd(), ":@localhost 473 " + cli.get_nick() + " " + channelName + " :Invite only channel");
		}
		else 									//OFF invite mode
			checkPassJoinOrReturn(i, cli, channelName, pass);
	}
	} 


	//std::vector<Channel>::iterator it = std::find(_channels.begin(), _channels.end(), channelName);
	unsigned int i = 0;
	int flag_channelname = 0;
	int flag_nickfound = 0;
	

	// Se esse canal tiver limiteMode ligado (limite de quantidade de clients no canal) e superior
	/*if (flag_channelname){
		if (_channels[i].getClientLimitChannelModeAndValue() && _channels[i].getClientsList().size() >= (size_t)_channels[i].getClientLimitChannelModeAndValue()) {
			sendMsgToClient(cli.GetFd(), ":@localhost 471 " + cli.get_nick() + " " + channelName + " :Cannot join channel (+l)");
			return ;
		}
		if (_channels[i].getInviteOnlyChannelMode()) {	//ON, invite mode
		
			unsigned int x = 0;
			for (; _channels[i].getInvCliList().size() ; x++){
				if (_channels[i].getInvCliList()[x].get_nick() == cli.get_nick()){
					flag_nickfound++;
					break;
				}
			}
			if (flag_nickfound)	
				checkPassJoinOrReturn(i, cli, channelName, pass);
			else										
				sendMsgToClient(cli.GetFd(), ":@localhost 473 " + cli.get_nick() + " " + channelName + " :Invite only channel");
		}
		else 									//OFF invite mode
			checkPassJoinOrReturn(i, cli, channelName, pass);
	}
	else {								//IF CHANNEL DOES NOT EXIST, create it
		this->_channels.push_back(Channel(channelName));
		Channel& newChannel = _channels.back();
		sendlMsgToChannel(newChannel.getClientsList(), ":" + cli.get_nick() + "!" + cli.get_user() + "@localhost" + " JOIN " + channelName + "\r\n");
		sendMsgToClient(cli.GetFd(), ":@localhost 332 " + cli.get_nick() + " " + channelName + " :No topic is set" + "\r\n");
	}*/


bool Server::verify_channelName(std::string channelName, std::vector<std::string> cmd, Client& cli){

	if (cmd.size() < 2) {
		sendMsgToClient(cli.GetFd(), ":@localhost 461 " + cli.get_nick() + " JOIN :Not enough parameters\n");
		return 0;
	}else if (channelName[0] != '#') { // Se não começar por #, primeiro char
		sendMsgToClient(cli.GetFd(), ":@localhost 403 " + cli.get_nick() + " " + cmd[1] + " No such channel\n");
		return 0;
	}else if (cli.is_verified()) {
		sendMsgToClient(cli.GetFd(), ":@localhost 463 " + cli.get_nick() + " JOIN :Already registred\r\n");
		return 0;
	}else if (channelName.size() == 1) {
		sendMsgToClient(cli.GetFd(), ":@localhost 403 " + cli.get_nick() + " " + cmd[1] + " No such channel\n");
		return 0;
	}else if (channelName.size() > 50) {
		sendMsgToClient(cli.GetFd(), ":@localhost 475 " + cli.get_nick() + " " + cmd[1] + " :Channel name too long\n");
		return 0;
	} else
		return 1;
}

int Server::channel_exists(std::string channelName){
	unsigned int i = 0;
	for (; i <_channels.size() ; i++){
		if (_channels[i].getChannelName() == channelName)
			return i;
	}
	return -1;
}

int Server::check_invite_list(int i, Client& cli){
	unsigned int x = 0;
		for (; _channels[i].getInvCliList().size() ; x++){
			if (_channels[i].getInvCliList()[x].get_nick() == cli.get_nick()){
					return 1;
				}
			}
	return 0;
}