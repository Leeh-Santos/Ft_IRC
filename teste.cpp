# include <string> 

# include <iostream>

int main() {
 
    std::string charsToExclude = " ,!"; // Characters to exclude from the search
    std::string str = "pass meucu";
    //size_t found = str.find_first_not_of("Helo");
    //str = str.substr(str.find_first_not_of("pass "));

    str = str.substr(str.find_first_not_of("pass "));

    std::cout << str << std::endl;


    return 0;
}
