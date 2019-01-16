#pragma once

#include "generator.hpp"

namespace modegen {
namespace generators {

class json_tree : public modegen::generator
{
	boost::property_tree::ptree opts;
public:
	boost::property_tree::ptree& options() override {return opts;}
	const boost::property_tree::ptree& options() const override {return opts;}

	void generate(mod_selection query, std::vector<modegen::module> mods) const override ;
};

} // namespace generators
} // namespace modegen
