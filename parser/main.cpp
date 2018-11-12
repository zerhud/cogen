#include <fstream>
#include <iostream>
#include <functional>
//#include <cppjson/json.h>

#include "modegen.hpp"
#include "grammar.hpp"

void print_mod(const modegen::module& mod)
{
	std::cout << mod.name << ":" << std::endl << "\t" << mod.content.size() << std::endl;

	for(auto& cnt:mod.content) {
		auto* fnc = std::get_if<modegen::function>(&cnt);
		if(fnc) {
			std::cout << "\tfunc: " << fnc->return_type.name << " " << fnc->name << " " << fnc->mut << " -- ";
			for(auto& p:fnc->params) std::cout << p.param_type.name << " " << p.name << ", ";
			std::cout << std::endl;
		}
	}

	std::cout << std::endl;
}

int main(int,char**)
{
	std::fstream file("pdata");
	std::string pdata{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
	auto begin = pdata.begin();
	auto end = pdata.end();

	std::vector<modegen::module> result;

	modegen::grammar<decltype(begin)> mg_parser;
	bool r = boost::spirit::qi::phrase_parse(begin,end, mg_parser, boost::spirit::ascii::space, result);

	if(!r) return 1;

	for(auto& mod:result) print_mod(mod);

	return 0;
}
