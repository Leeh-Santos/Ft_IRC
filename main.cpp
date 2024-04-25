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
    (void)port;
  

    //  port_int < 1024 || port_int > 49151 || s[0] != '\0'

    Server ser;
    
    std::cout << "---- SERVER ----" << std::endl;
    try{
        signal(SIGINT, Server::SignalHandler); //-> catch the signal (ctrl + c)
        signal(SIGQUIT, Server::SignalHandler); //-> catch the signal (ctrl + \)
        ser.ServerInit(); //-> initialize the server
    }
    catch(const std::exception& e){
        ser.CloseFds(); //-> close the file descriptors
        std::cerr << e.what() << std::endl;
    }
    std::cout << "The Server Closed!" << std::endl;

}