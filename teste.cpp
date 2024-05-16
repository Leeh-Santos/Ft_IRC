# include <string> 

# include <iostream>

int main() {
 
	std::string charsToExclude = ""; // Characters to exclude from the search
	charsToExclude += "vai caralho";
	charsToExclude += " seu arrombadinho";
	//size_t found = str.find_first_not_of("Helo");
	//str = str.substr(str.find_first_not_of("pass "));

  

	std::cout << charsToExclude << std::endl;


	return 0;
}
