#include "keyval_parser.hpp"
#include <vector>
#include <map>
#include <iostream>

int main(int argc, char* argv[]) {
	std::cout << "---------------------------------------" << std::endl;
	std::cout << "vector" << std::endl;
	std::cout << "---------------------------------------" << std::endl;
	std::vector<keyval_parser::value_type> ctr1;
	keyval_parser p1('[', ']', '=', '(', ')', ';', &ctr1);
	p1.parse("[a]=(b);", 8);
	p1.parse("  [a  a]  =  (b  b)  ;  ", 24);
	p1.parse("  [a  a]  =  (b  b)  ;  ", 24);
	p1.parse("  [a  a  =  (b  b  ;  ", 22);

	for(auto pair: ctr1) {
		std::cout << "\"" << pair.first << "\" = \"" << pair.second << "\"" << std::endl;
	}
	std::cout << "---------------------------------------" << std::endl;
	std::cout << "callback" << std::endl;
	std::cout << "---------------------------------------" << std::endl;
	std::map<keyval_parser::key_type, keyval_parser::val_type> ctr2;
	keyval_parser p2('\0', '\0', ':', '\0', '\0', '\n', [] (keyval_parser::value_type pair) {
		std::cout << "\"" << pair.first << "\" = \"" << pair.second << "\"" << std::endl;
	});
	p2.parse("a:b\r\n", 4);
	p2.parse("  a  :  b\r\n", 11);

	std::cout << "---------------------------------------" << std::endl;
	std::cout << "multimap" << std::endl;
	std::cout << "---------------------------------------" << std::endl;
	std::multimap<keyval_parser::key_type, keyval_parser::val_type> ctr3;
	keyval_parser p3('\0', '\0', ':', '\0', '\0', '\n', &ctr3);
	p3.parse("a:b\r\n", 4);
	p3.parse("  a  :  b\r\n", 11);

	for(auto pair: ctr3) {
		std::cout << "\"" << pair.first << "\" = \"" << pair.second << "\"" << std::endl;
	}
}