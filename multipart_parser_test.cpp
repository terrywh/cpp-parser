#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "multipart_parser.hpp"

// 也可以使用 vector<char> 存在 push_back(c) 等类型
typedef parser::multipart_parser<std::string> parser_type;

int main(int argc, char* argv[]) {
	std::string boundary("---abc");
	std::string data1("-----abc\r\ncontent-type: text/plain\r\ncontent-composition: name=\"abc\"; filename=\"abc.xyz\"\r\n\r\nthis data can be anything\r\n");
	std::string data2("-----abc\r\n\r\nthis data contain '---abc' boundary\r\n-----abc--");

	std::cout << "---------------------------------------" << std::endl;
	std::cout << "vector" << std::endl;
	std::cout << "---------------------------------------" << std::endl;
	std::vector<parser_type::entry_type> ctr1;
	parser_type p1(boundary, &ctr1);
	p1.parse(data1.c_str(), data1.size());
	p1.parse(data2.c_str(), data2.size());

	for(auto e: ctr1) {
		std::cout << "\"" << e.first << "\" = \"" << e.second << "\"" << std::endl;
	}
	std::cout << "---------------------------------------" << std::endl;
	std::cout << "callback" << std::endl;
	std::cout << "---------------------------------------" << std::endl;
	std::map<parser_type::value_type, parser_type::value_type> ctr2;
	parser_type p2(boundary, [] (parser_type::entry_type e) {
		std::cout << "\"" << e.first << "\" = \"" << e.second << "\"" << std::endl;
	});
	p2.parse(data1.c_str(), data1.size());
	p2.parse(data2.c_str(), data2.size());

	std::cout << "---------------------------------------" << std::endl;
	std::cout << "multimap" << std::endl;
	std::cout << "---------------------------------------" << std::endl;
	std::multimap<parser_type::value_type, parser_type::value_type> ctr3;
	parser_type p3(boundary, &ctr3);
	
	p3.parse(data1.c_str(), data1.size());
	p3.parse(data2.c_str(), data2.size());

	for(auto e: ctr3) {
		std::cout << "\"" << e.first << "\" = \"" << e.second << "\"" << std::endl;
	}
}