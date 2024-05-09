# include "Server.hpp"
# include "Client.hpp"

void Server::validate_cli(Client& cli){

    if (cli.get_nick() == "" || cli.get_user() == "" || cli.get_bool_pass() == 0){
        client_sender(cli.GetFd(), "YOUR REGISTRATION STATUS SO FAR:\n");
        if (cli.get_nick() == "")
            client_sender(cli.GetFd(), "Nick is missing\n");
        if (cli.get_user() == "")
            client_sender(cli.GetFd(), "User is missing\n");
        if (cli.get_bool_pass() == 0)
            client_sender(cli.GetFd(), "Password is missing\n");
    }
    else if (cli.get_nick() != "" && cli.get_user() != "" && cli.get_bool_pass() != 0) {
        cli.set_verified(1);
        client_sender(cli.GetFd(), "Welcome to IRC SERVER meu Parceiro\n");
    }
}

std::vector<std::string> Server::tokenit_please(std::string str){
    std::string sentence = str;

    std::istringstream iss(sentence);
    std::vector<std::string> tokens;
    std::string token;

    while (std::getline(iss, token, ' ') || std::getline(iss, token, '\n') || std::getline(iss, token, '\r')) {
        tokens.push_back(token);
    }
    //PRINT TOKENS TO VERIFY AFTER
    return tokens;
}

void Server::registration(std::string str, Client& cli){
    //talvez so foda se o arrombado ficar enviando CAP pelo nc e olhe lá, pode dar ruim na pass no compare
    if (str.find("CAP") != std::string::npos || str.find("PASS") != std::string::npos){
        std::vector<std::string> cap_tken_receiver = tokenit_please(str);

        std::vector<std::string>::iterator i = cap_tken_receiver.begin();
        std::cout << "size of tokens : " << cap_tken_receiver.size() << std::endl;
        std::cout << "tokens : ";
        while (i != cap_tken_receiver.end()){
            std::cout << *i << " ";
            i++;
        }
        if (cap_tken_receiver.size() <= 4){
            std::cout << "retornou" << std::endl;
            return ; // in case hexchat devide cap 
        }

        std::vector<std::string>::iterator it = std::find(cap_tken_receiver.begin(), cap_tken_receiver.end(), "PASS"); 
        std::string pass_str = *++it;
        std::cout << " explodiu aqui " << std::endl; 
        if(!pass_str.compare(serverpass)){
            if (cli.get_bool_pass()){
            std::cout << "Already registered" << std::endl;
            return;
            }
            cli.set_bool_pass(1);
            std::cout << "correct password!" << std::endl;
        }
        
        std::vector<std::string>::iterator it1 = std::find(cap_tken_receiver.begin(), cap_tken_receiver.end(), "NICK"); 
        cli.set_nick(*++it1);
        std::cout << " nick added :" << cli.get_nick() << std::endl;
        std::vector<std::string>::iterator it2 = std::find(cap_tken_receiver.begin(), cap_tken_receiver.end(), "USER");
        cli.set_user(*++it2);
        std::cout << " user added :" << cli.get_user() << std::endl;
        //oque eu faco com o resto que vem do CAP USER XXXXX  * realname??

    }
    else if (str.find("pass") != std::string::npos){
        std::cout << " entrou pass" << std::endl;
        str = str.substr(str.find_first_not_of("pass "));
        str = str.substr(0, str.find('\n')); // nc sends /n nesse caralho
        if (cli.get_bool_pass()){
            std::cout << "Already registered" << std::endl;
            client_sender(cli.GetFd(), "Already registered\n");
            return;
        }
        else if (!str.compare(serverpass)){
            cli.set_bool_pass(1);
            std::cout << "correct password!" << std::endl;
            client_sender(cli.GetFd(), "correct password!\n");
        }
        else{
            std::cout << "Wrong password" << std::endl;
            client_sender(cli.GetFd(), "Wrong password\n");
        }
    
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
        client_sender(cli.GetFd(), "Nick setted succesfully to :" + str + '\n');
    }
    else if (str.find("user") != std::string::npos){
        std::cout << " entrou user" << std::endl;
        str = str.substr(str.find_first_not_of("user "));
        str = str.substr(0, str.find('\n'));
        //check other nicks function
        if(str.empty())
            return;
        cli.set_user(str);
        std::cout << "user setted succesfully to :" << cli.get_user() << std::endl;
        client_sender(cli.GetFd(), "user setted succesfully to :" + str + '\n');
    }


    /*std::cout << "CLIENT INFO " << std::endl;
    std::cout << "CLIENT user :" << cli.get_user() <<std::endl;
    std::cout << "CLIENT nick :" << cli.get_nick() <<std::endl;
    std::cout << "CLIENT pass :" << cli.get_bool_pass() <<std::endl;*/
}