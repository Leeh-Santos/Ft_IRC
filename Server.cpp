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
void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl << "Signal Received! Ending server now" << std::endl;
	Server::_signal = true;
}

void	Server::CloseFds(){
	for(size_t i = 0; i < _clients.size(); i++){ //-> close all the clients
		std::cout << RED << "Client <" << _clients[i].GetFd() << "> Disconnected" << WHI << std::endl;
		close(_clients[i].GetFd());
	}
	if (_serSocketFd != -1){ //-> close the server socket
		std::cout << RED << "Server <" << _serSocketFd << "> Disconnected" << WHI << std::endl;
		close(_serSocketFd);
	}
}

void Server::ReceiveNewData(int fd, Client &cli)
{

	if (cli.GetFd() == _serSocketFd)
		std::cout << "deu ruim para localizar o cliente" << std::endl;

	char buff[1024];
	memset(buff, 0, sizeof(buff)); //-> clear the buffer to received data
	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1 , 0); //-> receive the data
	std::string in = buff;
	std::cout << "string received in general :" << in; //ja ta vindo com \n ou \r\n


	if(bytes <= 0){
		std::cout << RED << "Client <" << fd << "> Disconnected" << WHI << std::endl;
		ClearClients(fd);
		close(fd);
	}
	else if (!cli.is_verified()){ //travamos o gajo ate estiver ok!
			std::cout << YEL << "Client <" << fd << "> Data: " << WHI << in;
			if(in.find("\r\n") != std::string::npos){
				handle_cap(in, cli);
			}
			else{
				handle_nc(in, cli);
				client_sender(cli.GetFd(), "\nyou need to verify first Brother, usage: pass/nick/user 'input'\n");
			}
			validate_cli(cli); //verifica se ta tudo fixe para modificar o is_verified
		}
	else if (in.find("\r\n") == std::string::npos){ // se vem do nc commando
			std::cout << YEL << "Client <" << fd << "> Data: " << WHI << in;
			cli.set_buffer(in.substr(0, in.find_first_of('\n')));
			client_sender(cli.GetFd(), "messege received not from hexchat, added to buffer: " + in);
		}
	else { // verificar como ta chegando string pelo hexchat
		std::cout << YEL << "Client by HEXCHAT<" << fd << "> Data: " << WHI << in;
		std::string cli_str = cli.get_buffer() + in; //ta vindo com \r\n aqui cuidado
		if (cli_str.empty())
			return;
		cmd_execute(cli_str, cli);
		 // se ficar apertando enter que nem um retardado
		 //fazer comandos para PASS USER E NICK DENOVO, PASS E USER PARA ALREADY REGISTERED, nick para trocar outra vez, nao esquecer de fazer o announce
		//check command() function?
	}


}

void Server::AcceptNewClient()
{
	Client cli; //-> create a new client, dava para jogar isso tudo num constructor mas foda-se, norminete é o caralho, constructor = AcceptNewClient()
	struct sockaddr_in cliadd;
	struct pollfd NewPoll;
	socklen_t len = sizeof(cliadd);

	int incofd = accept(_serSocketFd, (sockaddr *)&(cliadd), &len); //-> accept the new client
	if (incofd == -1)
		{std::cout << "accept() failed" << std::endl; return;}

	if (fcntl(incofd, F_SETFL, O_NONBLOCK) == -1) //-> set the socket option (O_NONBLOCK) for non-blocking socket
		{std::cout << "fcntl() failed" << std::endl; return;}

	NewPoll.fd = incofd; //-> add the client socket to the pollfd
	NewPoll.events = POLLIN; //pollin = reading data
	NewPoll.revents = 0; //-> set the revents to 0

	cli.SetFd(incofd); //-> set the client file descriptor
	cli.setIpAdd(inet_ntoa((cliadd.sin_addr))); //-> convert the ip address to string and set it
	cli.set_verified(0);
	cli.set_user("");
	cli.set_bool_pass(0);
	cli.set_nick("");
	cli.set_buffer("");

	_clients.push_back(cli); //-> add the client to the vector of clients
	_fds.push_back(NewPoll); //-> add the client socket to the pollfd

	std::cout << GRE << "Client <" << incofd << "> Connected" << WHI << std::endl;
}

void Server::SerSocket()
{
	int en = 1;
	struct sockaddr_in add;
	struct pollfd NewPoll;
	add.sin_family = AF_INET; //->  ipv4
	add.sin_addr.s_addr = INADDR_ANY; //-> set the address to any local machine address
	add.sin_port = htons(this->_port); //-> convert the port to network byte order (big endian)

	_serSocketFd = socket(AF_INET, SOCK_STREAM, 0); //-> create the server socket
	if(_serSocketFd == -1) //-> check if the socket is created
		throw(std::runtime_error("faild to create socket"));

	if(setsockopt(_serSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1) //-> set the socket option (SO_REUSEADDR) to reuse the address
		throw(std::runtime_error("faild to set option (SO_REUSEADDR) on socket"));
	if (fcntl(_serSocketFd, F_SETFL, O_NONBLOCK) == -1) //-> set the socket option (O_NONBLOCK) for non-blocking socket
		throw(std::runtime_error("faild to set option (O_NONBLOCK) on socket"));
	if (bind(_serSocketFd, (struct sockaddr *)&add, sizeof(add)) == -1) //-> bind the socket to the address
		throw(std::runtime_error("faild to bind socket"));
	if (listen(_serSocketFd, SOMAXCONN) == -1) //-> listen for incoming connections and making the socket a passive socket
		throw(std::runtime_error("listen() faild"));

	NewPoll.fd = _serSocketFd; //-> add the server socket to the pollfd
	NewPoll.events = POLLIN; //-> set the event to POLLIN for reading data
	NewPoll.revents = 0; //-> set the revents to 0
	_fds.push_back(NewPoll); //-> add the server socket to the pollfd
}

Client& Server::get_client(int fd, std::vector<Client>& cli){

	for (unsigned int i = 0 ; i < cli.size() ; i++){
		if (fd 	== cli[i].GetFd())
			return cli[i];
	}
	return cli[0]; // just in case
}

void Server::ServerInit()
{
	std::cout << "Server Password :" << _serverPass << std::endl;
	SerSocket(); //-> create the server socket
	std::cout << GRE << "Server <" << _serSocketFd << "> Connected" << WHI << std::endl;
	std::cout << "Waiting to accept a connection...\n";

	while (Server::_signal == false){

		if((poll(&_fds[0],_fds.size(),-1) == -1) && Server::_signal == false)
			throw(std::runtime_error("poll() fail"));

		for (size_t i = 0; i < _fds.size(); i++){ //-> check all file descriptors
			if (_fds[i].revents & POLLIN){ //-> check if there is data to read
				if (_fds[i].fd == _serSocketFd) // se alteracao foi no fd do server entao so aceitamos
					AcceptNewClient();
				else
					ReceiveNewData(_fds[i].fd, get_client(_fds[i].fd, _clients)); //get_client devolve o cliente obj para modificarmos
			}
		}
	}
	CloseFds();
}



/*struct pollfd { server
 int	fd; //-> file descriptor
 short   events;//-> requested events
 short   revents; //-> returned events
};*/

