# include "Server.hpp"
# include "Client.hpp"

void Server::validate_cli(Client& cli){
    if (cli.get_nick() == "" || cli.get_user() == "" || cli.get_bool_pass() == 0){
        if (cli.get_nick() == "")
            client_sender(cli.GetFd(), "Nick is missing\n");
        if (cli.get_user() == "")
            client_sender(cli.GetFd(), "User is missing\n");
        if (cli.get_bool_pass() == 0)
            client_sender(cli.GetFd(), "Password is missing\n");
    }
    else{
        cli.set_verified(1);
        client_sender(cli.GetFd(), "Welcome to IRC SERVER meu Parceiro\n");
    }
}

void Server::registration(std::string str, Client& cli){

    if (str.find("CAP") != std::string::npos){
        std::cout << "CAP" << std::endl;
        //outro bagulho
    }
    else if (str.find("pass") != std::string::npos){
        std::cout << " entrou pass" << std::endl;
        str = str.substr(str.find_first_not_of("pass "));
        str = str.substr(0, str.find('\n')); // nc sends /n nesse caralho
        if (cli.get_bool_pass()){
            std::cout << "Already registered" << std::endl;
            return;
        }
        else if (!str.compare(serverpass)){
            cli.set_bool_pass(1);
            std::cout << "correct password! dont forget to register too" << std::endl;
        }
        else
            std::cout << "Wrong password" << std::endl;
    }
    else if (str.find("nick") != std::string::npos){
        std::cout << " entrou nick" << std::endl;
        str = str.substr(str.find_first_not_of("nick "));
        str = str.substr(0, str.find('\n'));
        //check other nicks
        if(str.empty())
            return;
        cli.set_nick(str);
        std::cout << "Nick setted succesfully to :" << cli.get_nick() << std::endl;
    }
    else if (str.find("user") != std::string::npos){
        std::cout << " entrou user" << std::endl;
        str = str.substr(str.find_first_not_of("user "));
        str = str.substr(0, str.find('\n'));
        //check other nicks
        if(str.empty())
            return;
        cli.set_user(str);
        std::cout << "user setted succesfully to :" << cli.get_user() << std::endl;
    }


    std::cout << "CLIENT INFO " << std::endl;
    std::cout << "CLIENT user :" << cli.get_user() <<std::endl;
    std::cout << "CLIENT nick :" << cli.get_nick() <<std::endl;
    std::cout << "CLIENT pass :" << cli.get_bool_pass() <<std::endl;
}