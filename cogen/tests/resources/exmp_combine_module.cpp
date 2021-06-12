#include "combine_module_2_0/declarations.hpp"

std::pmr::string combine_module::combine(std::pmr::vector<external_module::external_enum> what)
{
	std::pmr::string ret;
	for(auto& i:what) ret += to_string(i) + " ";
	return ret;
}
