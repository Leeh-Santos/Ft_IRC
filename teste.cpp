# include <string> 

# include <iostream>

int main() {
    std::string str = "Hello, World!";
    std::string charsToExclude = " ,!"; // Characters to exclude from the search

    size_t found = str.find_first_not_of("Helo");

    if (found != std::string::npos) {
        std::cout << "First character not in \"" << charsToExclude << "\" is at position: " << found << std::endl;
        std::cout << "Character found: " << str[found] << std::endl;
    } else {
        std::cout << "All characters in the string are in \"" << charsToExclude << "\"" << std::endl;
    }

    return 0;