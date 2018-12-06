#pragma once

#include "generator.hpp"

namespace modegen {
namespace generators {

class json_tree : public modegen::generator
{
public:
	void set_option(const std::string& key, const std::string& val) override ;
	void generate(mod_selection query, std::vector<modegen::module> mods) const override ;
};

} // namespace generators
} // namespace modegen
