#include <iostream>
#include "cs_1_0/server.hpp"

int main(int,char**)
{
	std::cout << "Ready to start cs server" << std::endl;
	while(true) {
		cs::step();
	}
	std::cout << "The cs server was finished!" << std::endl;
	return 0;
}

