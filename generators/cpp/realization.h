#pragma once

#include "generator.hpp"

namespace modegen {
namespace generators {
namespace cpp {

class realization : public modegen::generator
{
public:
	void generate(mod_selection query, std::vector<modegen::module> mods) const override ;
};

} // namespace cpp
} // namespace generators
} // namespace modegen
