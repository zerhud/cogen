#pragma once

#include <map>
#include <functional>
#include <filesystem>
#include <string_view>
#include <type_traits>

#include "modegen.hpp"

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
	module_content_selector sel;
	std::optional<std::filesystem::path> output;
};

//inline gen_options operator | (gen_options a, gen_options b)
//{
    //int ret = static_cast<int>(a) | static_cast<int>(b);
    //return static_cast<gen_options>(ret);
//}

//void cast_options(gen_options opts, std::vector<modegen::module>& mods);
void filter_by_selection(const mod_selection& query, std::vector<modegen::module>& mods);

class generator {
public:
	virtual ~generator() noexcept =default ;

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
