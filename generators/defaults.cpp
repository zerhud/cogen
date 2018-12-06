#include "defaults.h"

#include <cassert>

#include "json_tree.h"
#include "cpp/realization.h"

using namespace std::literals;

void modegen::generators::reg_default_generators(modegen::generator_maker* gm)
{
	assert(gm);

	auto cpp_realizatoin = [](){ return std::make_unique<cpp::realization>(); };
	gm->register_gen("server"sv, "cpp"sv, cpp_realizatoin);
	gm->register_gen("client"sv, "cpp"sv, cpp_realizatoin);
	gm->register_gen("server"sv, ""sv, cpp_realizatoin);
	gm->register_gen("client"sv, ""sv, cpp_realizatoin);

	gm->register_gen("json"sv, "json"sv, [](){ return std::make_unique<json_tree>(); });
	gm->register_gen("json"sv, ""sv, [](){ return std::make_unique<json_tree>(); });
}
