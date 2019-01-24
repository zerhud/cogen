#pragma once

#include "../generator.hpp"

namespace modegen::generation::cpp {

/// just c++ interface
class interface : public modegen::generator {
public:
	boost::property_tree::ptree& options() override { return opts; }
	const boost::property_tree::ptree& options() const override { return opts; }

	void output_name(std::string_view name, std::string_view value) override { outputs[name] = value; }

	void create_definitions(std::string_view part, std::vector<modegen::module> mods) const override ;

	void generate(mod_selection query, std::vector<modegen::module> mods) const override {}
private:
	boost::property_tree::ptree opts;
	std::map<std::string,std::string,std::less<>> outputs;
};

} // namespace modegen::generation::cpp
