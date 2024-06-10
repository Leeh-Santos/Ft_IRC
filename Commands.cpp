# include "Server.hpp"
# include "Client.hpp"

void Server::cmd_execute(std::string cli_str, Client& cli) {
	std::string first_word = cli_str.substr(0, cli_str.find(' '));
	first_word = first_word.substr(0, '\r');

	if (first_word == "pass" || first_word == "PASS")
		sendMsgToClient(cli.GetFd(), ":Server 462 " + cli.get_nick() + " :Already registered");
	else if (first_word == "user" || first_word == "USER")
		sendMsgToClient(cli.GetFd(), ":Server 462 " + cli.get_nick() + "!" + cli.get_user() + " :Already registered");
	else if (first_word == "nick" || first_word == "NICK")
		change_nick(cli_str, cli);
	else if (first_word == "join" || first_word == "JOIN")
		join_cmd(cli_str, cli);
	else if (first_word == "PRIVMSG" || first_word == "privmsg")
		privmsg_cmd(cli_str, cli);
	else if (first_word == "topic" || first_word == "TOPIC")
		topic_cmd(cli_str, cli);
	else{
		sendMsgToClient(cli.GetFd(), ":Server 421 " + cli.get_nick() + " " + first_word + ":Unknown command");
		//client_sender(cli.GetFd(), (":Server 421 " + cli.get_nick() + " " + first_word + ":Unknown command"));
		std::cout << "first word: " << first_word << "||| string inteira :" << cli_str << std::endl;
	}


}

void Server::change_nick(std::string cli_str, Client& cli){
	std::string nick = cli_str.substr(cli_str.find_first_not_of("nick "));
	nick = str_cutter(nick); // limpar a string
	if (verify_nicks(nick) != -1){
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
	//sendMsgToClient(cli.GetFd(), ":@localhost 332 " + cli.get_nick() + " " + channelName + " :" + _channels[i].getTopic());
	//sendlMsgToChannel(_channels[i].getClientsList(), ":" + cli.get_nick() + "!" + cli.get_user() + "@localhost" + " JOIN " + channelName);
	sendMsgToClient(cli.GetFd(), ":" + cli.get_nick() + "!" + channelName + " :" + _channels[i].getTopic());
	sendlMsgToChannel(_channels[i].getClientsList(), ":" + cli.get_nick() + "!" + cli.get_user() + "@localhost" + " JOIN " + channelName);
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

void	Server::join_cmd(std::string cmd_line, Client& cli) { //watch out with /r/n  join #asdasdas /r/n /join #a
	std::cout << " nao ta entrando" << std::endl;

	//cuidado em pass var, para nao dar merda se so mandarem poucos args tipo /join #asdasd
	std::vector<std::string> cmd = tokenit_please(cmd_line, 1);
	std::string channelName = cmd[1];
	std::string  pass = cmd[2];

	std::cout << "Channel name string: " << channelName << std::endl;
	std::cout << "Channel pass string: " << pass << std::endl;

	if (!verify_channelName(channelName, cmd, cli))
		return;
	else if(channel_exists(channelName) == -1){ // se nao exite criamos, -1 nao temos canal
		std::cout << "ENTROU: canal sendo criado" << std::endl;
		this->_channels.push_back(Channel(channelName));//destructor called porque ta aparecendo aqui?
		std::cout << " destructors sendo chamados agora: " << std::endl;
		Channel& newChannel = _channels.back(); 
		sendlMsgToChannel(newChannel.getClientsList(), ":" + cli.get_nick() + "!" + cli.get_user() + "@localhost" + " JOIN " + channelName);
		sendMsgToClient(cli.GetFd(), ":@localhost 332 " + cli.get_nick() + " " + channelName + " :No topic is set");
		joinChannel(_channels.size() - 1, cli, channelName, 1);
	}
	else {
		std::cout << "ENTROU: canal ja existe, vamos entrar" << std::endl;
		unsigned int i = channel_exists(channelName); //se tiver o canal channel_exists() devolve o index
	
		if (_channels[i].getClientLimitChannelModeAndValue() && _channels[i].getClientsList().size() >= (size_t)_channels[i].getClientLimitChannelModeAndValue()) {
			sendMsgToClient(cli.GetFd(), ":@localhost 471 " + cli.get_nick() + " " + channelName + " :Cannot join channel (+l)");
			return ;
		}
		else if(client_in_channel(cli.get_nick(), i)){
			
			sendMsgToClient(cli.GetFd(), ":@localhost 443 " + cli.get_nick() + ": You're already on that channel");	//);
			return;
		}
		else if (_channels[i].getInviteOnlyChannelMode()) {	//ON, invite mode
			if (check_invite_list(i, cli) != -1) // se tiver na list -1 nao esta
				checkPassJoinOrReturn(i, cli, channelName, pass);
			else										
				sendMsgToClient(cli.GetFd(), ":@localhost 473 " + cli.get_nick() + " " + channelName + " :Invite only channel\r\n");
		}
		else 									//OFF invite mode
			checkPassJoinOrReturn(i, cli, channelName, pass);
	}



	for(unsigned int i = 0 ; i < _channels.size() ; i++)
		std::cout << "nome do canal" << i << ": " << _channels[i].getChannelName() << std::endl;
	} 

bool Server::verify_channelName(std::string channelName, std::vector<std::string> cmd, Client& cli){

	if (cmd.size() < 2) {
		sendMsgToClient(cli.GetFd(), ":461 " + cli.get_nick() + " JOIN :Not enough parameters");
		return 0;
	}else if (channelName[0] != '#') { // Se não começar por #, primeiro char
		sendMsgToClient(cli.GetFd(), ":Server 403 " + cli.get_nick() + " " + cmd[1] + ":No such channel");
		return 0;
	}else if (channelName.size() == 1) {
		sendMsgToClient(cli.GetFd(), ":Server 403 " + cli.get_nick() + " " + cmd[1] + ":No such channel");
		return 0;
	}else if (channelName.size() > 50) {
		//sendMsgToClient(cli.GetFd(), ":@localhost 475 " + cli.get_nick() + " " + cmd[1] + ": Channel name too long");
		sendMsgToClient(cli.GetFd(), ":Server 475 " + cli.get_nick() + " " + cmd[1] + ":Channel name too long");
		return 0;
	} else
		return 1;
}

void 		Server::privmsg_cmd(std::string cli_str, Client& cli){

	std::vector<std::string> cmd = tokenit_please(cli_str, 1);

	if(cmd.size() < 3){
		sendMsgToClient(cli.GetFd(), ":localhost 461 " + cli.get_nick() + "Need more parameters");
		return;
	}
	std::string target = cmd[1];
	int chan_exists = channel_exists(target); //returns -1 if doesn't exist
	int nick_exists = verify_nicks(target); //returns -1 if doesn't exist
	std::string msg = get_full_msg(cmd, 2); //cuidado se vem menos strings

	if (target[0] != '#'){ //then wants to send to a user no inside a channel
		if(nick_exists == -1 || !_clients[nick_exists].is_verified()){
			sendMsgToClient(cli.GetFd(), ":localhost 401 " + cli.get_nick() + " " + target + " :No such nick");
			return;
		}
		else
			sendMsgToClient(_clients[nick_exists].GetFd(), ":" + cli.get_nick() + "!" + cli.get_user() + " PRIVMSG " + target + " " + msg);
	}
	else if(chan_exists == -1){
		sendMsgToClient(cli.GetFd(), ":localhost 403 " + cli.get_nick() + " " + target + " :No such channel");
		return;
	}
	else if(!client_in_channel(cli.get_nick(), chan_exists)){
		sendMsgToClient(cli.GetFd(), ":localhost 442 " + cli.get_nick() + " " + target + " :You're not on that channel");
		return;
	}
	else{
		sendlMsgToChannel2(_channels[chan_exists].getClientsList(), ":" + cli.get_nick() + "!" + cli.get_user() + " " + msg, cli); //another version
	}
}

int Server::channel_exists(std::string channelName){
	std::cout << " entrou channel_exists()" << std::endl;
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
					return x;
				}
			}
	return -1;
}

int Server::client_in_channel(std::string cli_nick, int index){

	Channel& chan = _channels[index];
	std::vector<Client> tmplist = chan.getClientsList();

	unsigned int i = 0; 

	for(; i < tmplist.size() ; i++){
		if (tmplist[i].get_nick() == cli_nick)
			return 1;
	}
	return 0;

}

std::string Server::get_full_msg(std::vector<std::string> cmd, int i){
	unsigned int x = i;
	std::string message = cmd[x];
	x++;
	for(; x < cmd.size() ; x++)
		message += " " + cmd[x];
	return message;
}

void Server::topic_cmd(std::string cli_str, Client &cli){ // TOPIC #nomedochanel nome do topicp 

	std::vector<std::string> cmd = tokenit_please(cli_str, 1);
	//topic_cmd could be fucked up if some one try to nc the server with a #chanel that is not in

	if(cmd.size() < 2){
		sendMsgToClient(cli.GetFd(), ":Server 461 " + cli.get_nick() + " :Not enough parameters");
		return;
	}
	std::string chan_name = cmd[1];
	int index = channel_exists(chan_name);

	//:luna.AfterNET.Org 332 Le #a :trocou tudo
	//:luna.AfterNET.Org 333 Le #a Lea!Lea@1DDA9:8857D9:9E2AD0:3C77BE:IP 1717775316	
	
	if (channel_exists(chan_name) == -1){
		sendMsgToClient(cli.GetFd(), ":Server 332 " + cli.get_nick() + " " + chan_name + ":No such channel");
		return;
	}
	if (!client_in_channel(cli.get_nick(), index)){
		sendMsgToClient(cli.GetFd(), ":Server 332 " + cli.get_nick() + " " + chan_name + ":You are not on this fucking channel bro, stop forcing my parsing thru nc");
		return;
	}
	if(cmd.size() == 2){
		if (_channels[index].getTopic() == "") //se existe topic ou nao para mandar 
			sendMsgToClient(cli.GetFd(), ":Server 331 " + cli.get_nick() + " " + chan_name + ":No topic is set."); // :luna.AfterNET.Org 331 lea #asd :No topic is set.
		else{
			sendMsgToClient(cli.GetFd(), ":Server 332 " + cli.get_nick() + " " + chan_name + " :" + _channels[index].getTopic());
			sendMsgToClient(cli.GetFd(), ":Server 333 " + cli.get_nick() + " " + chan_name + " " + _channels[index].getOperator());
		}		
	}
	else if (cmd.size() >= 3){
		std::string msg = get_full_msg(cmd, 2);
		if (_channels[index].getOperator() != cli.get_nick()){ //check if the client is the operator
			if(_channels[index].getTopicMode() == false){
				sendMsgToClient(cli.GetFd(), ":Server 482 " + cli.get_nick() + " " + chan_name + ":You're not channel operator");
				return;
			}
			else{
				_channels[index].setTopic(msg);
				sendlMsgToChannel(_channels[index].getClientsList(), ":" + cli.get_nick() + "!" + cli.get_user() + "@localhost" + " TOPIC " + chan_name + " " + msg);
			}
		}//>> :Lea!Lea@E32A1D:BD8DBE:9E2AD0:3C77BE:IP TOPIC #ad :asdasd asd
		else{
			_channels[index].setTopic(msg);
			sendlMsgToChannel(_channels[index].getClientsList(), ":" + cli.get_nick() + "!" + cli.get_user() + "@localhost" + " TOPIC " + chan_name + " " + msg);\
				//:Lea!Lea@1DDA9:8857D9:9E2AD0:3C77BE:IP TOPIC #a :troca essa merda   - manda pa geral do canal do server pa trocar o nome
		}
	}


}
