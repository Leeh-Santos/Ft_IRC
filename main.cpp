# include "Client.hpp"
# include "Server.hpp"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (argc != 3)
        std::cout << "wrong usage meu parceiro, it goes like: ./irc <port> <server-pass>" << std::endl;

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
    Server ser(port, argv[2]);
    
    std::cout << "---- SERVER IN ONLINE EVERYONE  ----" << std::endl;
    try{
        signal(SIGINT, Server::SignalHandler);
        signal(SIGQUIT, Server::SignalHandler); 
        ser.ServerInit(); 
    }
    catch(const std::exception& e){
        ser.CloseFds(); 
        std::cerr << e.what() << std::endl;
    }
    std::cout << "The Server Closed! See you next time!" << std::endl;

}