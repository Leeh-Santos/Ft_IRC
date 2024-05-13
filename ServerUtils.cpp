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
            client_sender(cli.GetFd(), "Password is missing, if you are using hexchat, either restart app with correct pass or use the command /join PASS 'password'\n"); 
            //provavelmente fazer a mesma coisa ou com o nick ou com o user 
    }
    else if (cli.get_nick() != "" && cli.get_user() != "" && cli.get_bool_pass() != 0) {
        cli.set_verified(1);
        client_sender(cli.GetFd(), "Welcome to IRC SERVER meu Parceiro\n");
        std::cout << "Welcome to IRC SERVER meu Parceiro" << std::endl;
    }
}

std::vector<std::string> Server::tokenit_please(std::string str, int x){
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;

    if (x){ //for /r/n
        for (size_t i = 0; i < str.length(); ++i) {
            if (str[i] == ' ' || str[i] == '\r' || str[i] == '\n') {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            } else { 
                token += str[i];
            }
        }
        if (!token.empty()) {
            tokens.push_back(token);
        }
    } 
     else{ // for /n and spaces 
        while (std::getline(iss, token, ' ')) 
               tokens.push_back(token);
    }

    return tokens;
}


void Server::handle_cap(std::string str, Client& cli){

    std::vector<std::string> cap_tken_receiver = tokenit_please(str, 1);
   
    std::vector<std::string>::iterator it = std::find(cap_tken_receiver.begin(), cap_tken_receiver.end(), "PASS");
    if (it != cap_tken_receiver.end()){
        std::string pass_str = *++it;
        if(!pass_str.compare(serverpass)){
            if (cli.get_bool_pass()){
                std::cout << "Already registered" << std::endl;
                client_sender(cli.GetFd(), "Already registered\n");
                return;
            }
            cli.set_bool_pass(1);
            std::cout << "correct password!" << std::endl;
            client_sender(cli.GetFd(), "Correct passoword!\n");
        } else{
            std::cout << "incorrect password!" << std::endl;
            client_sender(cli.GetFd(), "incorrect password!\n");
        }
    }
    std::vector<std::string>::iterator it1 = std::find(cap_tken_receiver.begin(), cap_tken_receiver.end(), "NICK");
    if (it1 != cap_tken_receiver.end()){
        cli.set_nick(*++it1);
        std::cout << " nick added :" << cli.get_nick() << std::endl;
        client_sender(cli.GetFd(), "NICK added " + cli.get_nick() + '\n');
    }
    std::vector<std::string>::iterator it2 = std::find(cap_tken_receiver.begin(), cap_tken_receiver.end(), "USER");
    if (it2 != cap_tken_receiver.end()){
        cli.set_user(*++it2);
        std::cout << " user added :" << cli.get_user() << std::endl;
        client_sender(cli.GetFd(), "USER added " + cli.get_user() + '\n');
    }

    /*        std::vector<std::string>::iterator i = cap_tken_receiver.begin();
    std::cout << "size of tokens : " << cap_tken_receiver.size() << std::endl;
    std::cout << "tokens : ";
    while (i != cap_tken_receiver.end()){
        std::cout << *i << " ";
        i++;
    }*/
    

}
void Server::handle_nc(std::string str, Client& cli){
        if (str.find("pass") != std::string::npos){
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
