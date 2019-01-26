#pragma once

#include <map>
#include <functional>
#include <filesystem>
#include <string_view>
#include <type_traits>
#include <boost/property_tree/ptree.hpp>

#include "parser/modegen.hpp"
#include "converters/case.h"

namespace modegen {

class generator;
typedef std::function<std::unique_ptr<generator>()> generator_creator;

enum class gen_options {
	  no_opts = 1 << 0
	, split_version = 1 << 1
	, split_modules = 1 << 2
};

struct generation_request {
	cvt::name_conversion naming = cvt::name_conversion::underscore;
	gen_options opts = gen_options::no_opts;
	module_content_selector sel=module_content_selector::all;
	std::string mod_name; ///< module regex
	std::string cnt_name; ///< content regex
};

class generator {
public:
	virtual ~generator() noexcept =default ;

	virtual boost::property_tree::ptree& options() =0 ;
	virtual const boost::property_tree::ptree& options() const =0 ;

	/// set path needed for the generator. - is std::cout.
	/// @param name path to what
	/// @param value the path
	virtual void path(const std::string& name, const std::string& value) =0 ;

	/// generate output (definition for the interface).
	/// @param querty generation request (parameters) @param mods is declared interface
	virtual void create_definitions(generation_request query, std::vector<modegen::module> mods) const =0 ;
};

class generator_maker {
public:
	void register_gen(std::string_view part, std::string_view name, generator_creator crt);
	std::unique_ptr<generator> make_generator(std::string_view part, std::string_view name) const ;
private:
	static std::string make_path(std::string_view part, std::string_view name) ;
	std::map<std::string, generator_creator> gens;
};

} // namespace modegen
