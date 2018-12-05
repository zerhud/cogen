#include "defaults.h"

#include <cassert>

#include "json_tree.h"
#include "cpp/declarations_gen.h"
#include "cpp/realization.h"

using namespace std::literals;

void modegen::generators::reg_default_generators(modegen::generator_maker* gm)
{
	assert(gm);

	gm->register_gen("cpp"sv, "declarations"sv, [](){ return std::make_unique<cpp::declarations>(); });
	gm->register_gen("cpp"sv, ""sv, [](){ return std::make_unique<cpp::declarations>(); });

	gm->register_gen("cpp"sv, "realization"sv, [](){ return std::make_unique<cpp::realization>(); });

	gm->register_gen("json"sv, "json"sv, [](){ return std::make_unique<json_tree>(); });
	gm->register_gen("json"sv, ""sv, [](){ return std::make_unique<json_tree>(); });
}
