# include "Server.hpp"
# include "Client.hpp"

void Server::registration(std::string str, Client& cli){
    if (str.find("CAP") != std::string::npos){
        //outro bagulho
    }
    else if (str.find("pass") != std::string::npos){
        if (cli.get_bool_pass()){
            std::cout << "Already registered" << std::endl;
            return;
        }
        str = str.substr(str.find_first_not_of("pass "));
        if (!str.compare(serverpass))
            std::cout << "Wrong password" << std::endl;
        else {
            cli.set_bool_pass(1);
            std::cout << "correct password! dont forget to register too" << std::endl;
        }  

    }
    else if (str.find("nick") != std::string::npos){
        str = str.substr(str.find_first_not_of("nick "));
        //check other nicks
        if(str.empty())
            return;
        cli.set_nick(str);
        
        std::cout << "Nick setted succesfully to :" << cli.get_nick() << std::endl;
    }
    else if (str.find("user") != std::string::npos){
        str = str.substr(str.find_first_not_of("user "));
        //check other nicks
        if(str.empty())
            return;
        cli.set_user(str);
        std::cout << "user setted succesfully to :" << cli.get_user() << std::endl;
    }
}