#include <cassert>
#include <iostream>
#include "srv.hpp"

int main(int, char**)
{
	mod_v1_0::ugly_interface_ptr uptr = mod_v1_0::create_ugly_interface("param");
	assert( uptr->get_param() == "param");

	std::cout << mod_v1_0::e1::one << std::endl;
	return 0;
}
