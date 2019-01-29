#pragma once

#include <filesystem>
#include <cppjson/json.h>
#include "../generator.hpp"

namespace modegen::generation::cpp {

/// just c++ interface
class interface : public modegen::generator {
public:
	interface(std::shared_ptr<generation_provider> p);

	boost::property_tree::ptree& options() override { return opts; }
	const boost::property_tree::ptree& options() const override { return opts; }

	void path(const std::string& name, const std::string& value) override { outputs[name] = value; }

	void create_definitions(generation_request query, std::vector<modegen::module> mods) const override ;
private:
	std::filesystem::path tmpl_path(std::string_view tname) const ;
	std::filesystem::path path(std::string_view part) const ;
	std::string part_name(std::string_view part) const ;
	cppjson::value add_includes(std::vector<std::string> incs, cppjson::value val) const ;

	boost::property_tree::ptree opts;
	std::shared_ptr<generation_provider> provider;

	std::map<std::string,std::filesystem::path,std::less<>> outputs;
};

} // namespace modegen::generation::cpp
