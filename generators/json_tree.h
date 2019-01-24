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

	void output_name(std::string_view name, std::string_view value) override {}
	void create_definitions(std::string_view part, std::vector<modegen::module> mods) const override {}
};

} // namespace generators
} // namespace modegen
