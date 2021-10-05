#include <thread>
#include <iostream>
#include "cs_1_0/server.hpp"

using namespace std::literals;

int main(int,char**)
{
	std::cout << "Ready to start cs server" << std::endl;
	while(cs::step()) {
		std::this_thread::sleep_for(1s);
	}
	std::cout << "The cs server was finished!" << std::endl;
	return 0;
}

