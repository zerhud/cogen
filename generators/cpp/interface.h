#pragma once

#include <filesystem>
#include "../generator.hpp"

namespace modegen::generation::cpp {

/// just c++ interface
class interface : public modegen::generator {
public:
	boost::property_tree::ptree& options() override { return opts; }
	const boost::property_tree::ptree& options() const override { return opts; }

	void output_name(const std::string& name, const std::string& value) override { outputs[name] = value; }

	void create_definitions(generation_request query, std::vector<modegen::module> mods) const override ;

	void generate(mod_selection query, std::vector<modegen::module> mods) const override {}
private:
	std::filesystem::path path(std::string_view part) const ;

	boost::property_tree::ptree opts;
	std::map<std::string,std::filesystem::path,std::less<>> outputs;
};

} // namespace modegen::generation::cpp
