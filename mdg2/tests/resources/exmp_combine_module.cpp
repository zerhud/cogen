#include <combine_module_export.h>
#define DLL_EXPORT COMBINE_MODULE_EXPORT
#include "combine_module_2_0/declarations.hpp"

std::pmr::string combine_module_2_0::combine(std::pmr::vector<external_module_1_1::external_enum> what)
{
	std::pmr::string ret;
	for(auto& i:what) ret += to_string(i) + " ";
	return ret;
}
