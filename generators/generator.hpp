#pragma once

#include <map>
#include <functional>
#include <filesystem>
#include <string_view>
#include <type_traits>
#include <boost/property_tree/ptree.hpp>

#include "parser/modegen.hpp"

namespace modegen {

class generator;
typedef std::function<std::unique_ptr<generator>()> generator_creator;

enum class gen_options {
	  no_opts = 1 << 0
	, split_version = 1 << 1
};

struct mod_selection {
	gen_options opts;
	std::string mod_name;
	std::string cnt_name;
	module_content_selector sel=module_content_selector::all;

	std::string what_generate;
	std::optional<std::filesystem::path> output;
};

struct generator_request {
	std::vector<modegen::module> data;
	boost::property_tree::ptree options;
	mod_selection selector;
};

generator_request& operator | (generator_request& req, const std::function<void(generator_request&)>& gen);

//void cast_options(gen_options opts, std::vector<modegen::module>& mods);
void filter_by_selection(const mod_selection& query, std::vector<modegen::module>& mods);

namespace generation {
/// applay filter_by_selection to the request
void select(generator_request& req);
} // namespace generation

class generator {
public:
	virtual ~generator() noexcept =default ;

	virtual boost::property_tree::ptree& options() =0 ;
	virtual const boost::property_tree::ptree& options() const =0 ;

	virtual void generate(mod_selection query, std::vector<modegen::module> mods) const =0 ;
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
