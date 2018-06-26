#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "concepts.hpp"

int main(int argc, char* argv[]) {
	std::cout << "is_char_pushable<std::string> = " << parser::concepts::is_char_pushable<std::string>::value << std::endl;
	std::cout << "is_char_pushable<std::vector<char>> = " << parser::concepts::is_char_pushable<std::vector<char>>::value << std::endl;
	std::cout << "is_char_pushable<int> = " << parser::concepts::is_char_pushable<int>::value << std::endl;
	
	std::cout << "is_entry_insertable<std::vector<std::string>> = " << parser::concepts::is_entry_insertable<std::vector<std::string>, std::string>::value << std::endl;
	std::cout << "is_entry_pushable<std::vector<std::string>> = " << parser::concepts::is_entry_pushable<std::vector<std::string>, std::string>::value << std::endl;

	std::cout << "is_entry_insertable<std::multimap<std::string, std::string>> = " << parser::concepts::is_entry_insertable<std::multimap<std::string, std::string>, std::pair<std::string, std::string>>::value << std::endl;
	std::cout << "is_entry_pushable<std::multimap<std::string, std::string>> = " << parser::concepts::is_entry_pushable<std::multimap<std::string, std::string>, std::pair<std::string, std::string>>::value << std::endl;
	return 0;
}