#pragma once

#include "generator.hpp"

namespace modegen {
namespace generators {

class json_tree : public modegen::generator
{
public:
	void generate(mod_selection query, std::vector<modegen::module> mods) const override ;
};

} // namespace generators
} // namespace modegen
