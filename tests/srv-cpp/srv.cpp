#include <cassert>
#include <sstream>
#include <iostream>
#include "srv.hpp"

std::vector<std::string_view> failed_tests;

void print_failed()
{
	if(!failed_tests.empty()) {
		std::cout << std::endl << "failed:" << std::endl;
		for(auto& f:failed_tests) std::cout << f << std::endl;
	}
}

void compare_str(std::string_view tn, const std::string& left, std::string_view right)
{
	if(left==right) std::cout << ".";
	else {
		std::cout << "!";
		failed_tests.emplace_back(tn);
	}
}

mod_v1_0::ugly_interface_ptr mod_v1_0::create_ugly_interface(std::string param)
{
	struct realization : mod_v1_0::ugly_interface {
		std::string p;
		realization(std::string param) : p(std::move(param)) {}
		std::string get_param() const override { return p; }
	};

	return std::make_shared<realization>(std::move(param));
}

int main(int, char**)
{
	mod_v1_0::ugly_interface_ptr uptr = mod_v1_0::create_ugly_interface("param");
	compare_str("ugly_interface param", uptr->get_param(), "param");

	std::stringstream e1_test;
	e1_test << mod_v1_0::e1::one << " " << mod_v1_0::e1::two ;
	compare_str("mod::e1 putputs", e1_test.str(), "one two");

	print_failed();
	return 0;
}
