# include "Server.hpp"
# include "Client.hpp"

void Server::ClearClients(int fd){ //-> clear the clients
	for(size_t i = 0; i < _fds.size(); i++){
		if (_fds[i].fd == fd)
			{_fds.erase(_fds.begin() + i); break;}
	}
	for(size_t i = 0; i < _clients.size(); i++){
		if (_clients[i].GetFd() == fd)
			{_clients.erase(_clients.begin() + i); break;}
	}

}

bool Server::_signal = false;
void Server::inbound_signal(int sig)
{
	(void)sig;
	std::cout << std::endl << "Inbound signal! Server will be closed" << std::endl;
	Server::_signal = 1;
}

void	Server::CloseSocket(){
	for(size_t i = 0; i < _clients.size(); i++){ 
		std::cout << RED << "Client <" << _clients[i].GetFd() << "> Disconnected" << WHI << std::endl;
		close(_clients[i].GetFd());
	}
	if (_serSocketFd != -1){ 
		std::cout << RED << "Server <" << _serSocketFd << "> Disconnected" << WHI << std::endl;
		close(_serSocketFd);
	}
}

void Server::clientRequest(int fd, Client &cli)
{

	if (cli.GetFd() == _serSocketFd)
		std::cout << "not able to locate client" << std::endl;

	char buff[1024];
	memset(buff, 0, sizeof(buff)); //-> clear the buffer for new msgs
	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1 , 0);
	std::string in = buff; 
	
	if(bytes <= 0){
		std::cout << RED << "Client <" << fd << "> Disconnected" << WHI << std::endl;
		quit_cmd(cli);
		ClearClients(fd);
		//remove_client(cli); already in quit_cmd()
		close(fd);
	}
	else if (!cli.is_verified()){
			std::cout << YEL << "Client <" << fd << "> Data: " << WHI << in;
			if(in.find("\r\n") != std::string::npos){
				handle_cap(in, cli);
			}
			else{
				handle_nc(in, cli);
				client_sender(cli.GetFd(), "\nyou need to verify first Brother, usage: pass/nick/user 'input'\n");
			}
			validate_cli(cli);
		}
	else if (in.find("\r\n") == std::string::npos){ 
			std::cout << YEL << "Client <" << fd << "> Data: " << WHI << in;
			cli.set_buffer(in.substr(0, in.find_first_of('\n')));
			client_sender(cli.GetFd(), "messege received not from hexchat, added to buffer: " + in);
		}
	else { 
		std::cout << YEL << "Client by HEXCHAT<" << fd << "> Data: " << WHI << in;
		std::string cli_str = cli.get_buffer() + in; 
		if (cli_str.empty())
			return;
		cmd_execute(cli_str, cli);
		cli.refresh_buffer();
	}

}

void Server::remove_client(Client& cli){
	/*std::vector<Client>::iterator it = _clients.begin();

	for(; it != _clients.end(); it++){
		if(it->get_nick() == cli.get_nick()){
			_clients.erase(it);
			break;
		}
	}*/

	for (std::vector<Client>::iterator i = this->_clients.begin(); i != this->_clients.end(); i++)
	{
		if (i->GetFd() == cli.GetFd())
		{
			this->_clients.erase(i);
			break;
		}
	}
	/*for (std::vector<struct pollfd>::iterator i = this->_fds.begin(); i != this->_fds.end(); i++)
	{
		if (i->fd == cli.GetFd())
		{
			this->_fds.erase(i);
			break;
		}
	}*/

}

void Server::connectionRequest()
{
	Client cli; //-> create a new client, f** norminette constructor = connectionRequest()
	struct sockaddr_in socket_info;
	socklen_t len = sizeof(socket_info);

	int cli_fd = accept(_serSocketFd, (sockaddr *)&(socket_info), &len); //-> accept the new client
	if (cli_fd == -1)
		{std::cout << "accept() function failed" << std::endl; return;}

	if (fcntl(cli_fd, F_SETFL, O_NONBLOCK) == -1) //-> set the socket option (O_NONBLOCK) for non-blocking socket
		{std::cout << "fcntl() function failed" << std::endl; return;}

	set_serverTracker(cli_fd);

	cli.SetFd(cli_fd); //-> set the client file descriptor
	cli.setIpAdd(inet_ntoa((socket_info.sin_addr))); //-> convert the ip address to string and set it
	cli.set_verified(0);
	cli.set_user("");
	cli.set_bool_pass(0);
	cli.set_nick("");
	cli.set_buffer("");
	_clients.push_back(cli); //-> add the client to the vector of clients

	std::cout << GRE << "Client <" << cli_fd << "> Connected" << WHI << std::endl;
}

void Server::ServerSocket()
{
	int en = 1;
	struct sockaddr_in socket_info;
	socket_info.sin_family = AF_INET; //->  ipv4
	socket_info.sin_addr.s_addr = INADDR_ANY; //-> any local host address
	socket_info.sin_port = htons(this->_port); //-> convert the port to network byte order (big endian)

	_serSocketFd = socket(AF_INET, SOCK_STREAM, 0); //-> create the server socket
	if(_serSocketFd == -1) //-> check if the socket is created
		throw(std::runtime_error("failed to create socket"));
	if(setsockopt(_serSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1) //-> set the socket option (SO_REUSEADDR) to reuse the address
		throw(std::runtime_error("failed to set option (SO_REUSEADDR) on socket"));
	if (fcntl(_serSocketFd, F_SETFL, O_NONBLOCK) == -1) //-> set the socket option (O_NONBLOCK) for non-blocking socket
		throw(std::runtime_error("failed to set option (O_NONBLOCK) on socket"));
	if (bind(_serSocketFd, (struct sockaddr *)&socket_info, sizeof(socket_info)) == -1) //-> bind the socket to the address
		throw(std::runtime_error("failed to bind socket"));
	if (listen(_serSocketFd, SOMAXCONN) == -1) //-> listen for incoming connections and making the socket a passive socket
		throw(std::runtime_error("listen() faild"));

	set_serverTracker(_serSocketFd);
	
}

void	Server::set_serverTracker(int fd){
	struct pollfd NewPoll;
	NewPoll.fd = fd; 
	NewPoll.events = POLLIN; 
	NewPoll.revents = 0; 
	_fds.push_back(NewPoll); 
}

Client& Server::get_client(int fd, std::vector<Client>& cli){

	for (unsigned int i = 0 ; i < cli.size() ; i++){
		if (fd 	== cli[i].GetFd())
			return cli[i];
	}
	return cli[0]; // just in case
}

void Server::start_Server()
{
	std::cout << "Server Password :" << _serverPass << std::endl;
	ServerSocket(); 
	std::cout << GRE << "Server <" << _serSocketFd << "> Connected" << WHI << std::endl;
	std::cout << "listening and waiting to accept a connection...\n";

	while (_signal == 0){

		if((poll(&_fds[0],_fds.size(),-1) == -1) && Server::_signal == 0)
			throw(std::runtime_error("poll() fail"));

		for (size_t i = 0; i < _fds.size(); i++){ //-> check all file descriptors
			if (_fds[i].revents & POLLIN){ //-> check if there is data to read
				if (_fds[i].fd == _serSocketFd) // se alteracao foi no fd do server entao so aceitamos
					connectionRequest();
				else
					clientRequest(_fds[i].fd, get_client(_fds[i].fd, _clients)); //get_client devolve o cliente obj para modificarmos
			}
		}
	}
	CloseSocket();
}



/*struct pollfd { server
 int	fd; //-> file descriptor
 short   events;//-> requested events
 short   revents; //-> returned events
};*/

	