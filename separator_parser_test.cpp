#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "separator_parser.hpp"

// 也可以使用 vector<char> 存在 push_back(c) 等类型
typedef parser::separator_parser<std::string, std::string> parser_type;

int main(int argc, char* argv[]) {
	std::cout << "---------------------------------------" << std::endl;
	std::cout << "vector" << std::endl;
	std::cout << "---------------------------------------" << std::endl;
	std::vector<parser_type::entry_type> ctr1;
	parser_type p1('[', ']', '=', '(', ')', ';', &ctr1);
	p1.parse("[a]=(b);", 8);
	p1.parse("  [a  a]  =  (b  b)  ;  ", 24);
	p1.parse("  [a  a]  =  (b  b)  ;  ", 24);
	p1.parse("  [a  a  =  (b  b  ;  ", 22);

	for(auto e: ctr1) {
		std::cout << "\"" << e.first << "\" = \"" << e.second << "\"" << std::endl;
	}
	std::cout << "---------------------------------------" << std::endl;
	std::cout << "callback" << std::endl;
	std::cout << "---------------------------------------" << std::endl;
	std::map<parser_type::key_type, parser_type::value_type> ctr2;
	parser_type p2('\0', '\0', ':', '\0', '\0', '\n', [] (parser_type::entry_type e) {
		std::cout << "\"" << e.first << "\" = \"" << e.second << "\"" << std::endl;
	});
	p2.parse("a:\r\n", 4);
	p2.parse("a:b\r\n", 5);
	p2.parse("  a  :  b\r\n", 11);

	std::cout << "---------------------------------------" << std::endl;
	std::cout << "multimap" << std::endl;
	std::cout << "---------------------------------------" << std::endl;
	std::multimap<parser_type::key_type, parser_type::value_type> ctr3;
	parser_type p3('\0', '\0', ':', '\0', '\0', '\n', &ctr3);
	p3.parse("a:b\r\n", 4);
	p3.parse("  a  :  b\r\n", 11);

	for(auto e: ctr3) {
		std::cout << "\"" << e.first << "\" = \"" << e.second << "\"" << std::endl;
	}
}
