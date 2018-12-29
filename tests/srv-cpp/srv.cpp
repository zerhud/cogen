#include <cassert>
#include <sstream>
#include <iostream>
#include "srv.hpp"

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
	if(uptr->get_param()=="param") std::cout << "." ;
	else std::cout << "!" ;

	std::stringstream e1_test;
	e1_test << mod_v1_0::e1::one << mod_v1_0::e1::two ;
	if(e1_test.str() == "onetwo") std::cout << "." ;
	else std::cout << "!" ;

	return 0;
}
