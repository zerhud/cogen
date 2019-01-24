#pragma once

#include <cppjson/json.h>
#include "../generator.hpp"
#include "../case_converter.h"

namespace modegen {
namespace generators {
namespace cpp {

class realization : public modegen::generator
{
public:
	realization();
	void generate(mod_selection query, std::vector<modegen::module> mods) const override ;

	boost::property_tree::ptree& options() override { return opts; }
	const boost::property_tree::ptree& options() const override { return opts; }

	void output_name(std::string_view name, std::string_view value) override {}
	void create_definitions(std::string_view part, std::vector<modegen::module> mods) const override {}
private:
	static std::string pythongen_path() ;
	static std::string tmpl_path(std::string_view tn) ;

	void gen_hpp(mod_selection query, std::vector<modegen::module> mods) const ;
	void gen_cpp(mod_selection query, std::vector<modegen::module> mods) const ;
	void gen_def(mod_selection query, std::vector<modegen::module> mods) const ;

	void set_out(std::filesystem::path base, std::string_view parsed_file, std::string_view part) const ;

	name_conversion naming_option() const ;
	std::string solve_option(std::string_view name) const ;
	bool solve_bool_option(std::string_view name) const ;
	void try_to_set_option(std::string_view name, std::string val);

	void generate(const cppjson::value& data, std::string_view t) const ;

	void add_extra_info(cppjson::value& cdata) const ;
	void set_constructors_prefix(cppjson::value& cdata) const ;
	void set_extra_namespaces(cppjson::value& cdata) const ;

	boost::property_tree::ptree opts;
	mutable std::filesystem::path out_path;
};

} // namespace cpp
} // namespace generators
} // namespace modegen
