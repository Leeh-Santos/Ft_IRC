# include "Client.hpp"
# include "Server.hpp"

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	if (argc != 3){
		std::cout << "wrong usage meu parceiro, it goes like: ./irc <port> <server-pass>" << std::endl;
		exit(1);
	}

	int i = 0;
	while (argv[1][i]){
		if (!isdigit(argv[1][i])){
			std::cout << "invalid port" << std::endl;
			exit(1);
		}
		i++;
	}

	int port = atoi(argv[1]);
	if (port < 1024 || port > 49151){
		std::cout << "invalid port" << std::endl;
		exit(1);
	}

	Server server(port, argv[2]);
	
	std::cout << "---- SERVER IS ONLINE EVERYONE  ----" << std::endl;
	try{
		signal(SIGINT, Server::inbound_signal);
		signal(SIGQUIT, Server::inbound_signal); 
		server.start_Server(); 
	}
	catch(const std::exception& e){
		server.CloseSocket(); 
		std::cerr << e.what() << std::endl;
	}
	std::cout << "Ending server! See you next time!" << std::endl;

}