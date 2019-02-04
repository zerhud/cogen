#pragma once

#include <vector>
#include <string>
#include <variant>
#include <optional>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include "meta_parameters.hpp"
#include "config.hpp"

namespace modegen::parser::interface {
struct type {
	type() =default ;
	type(const type& o) : name(o.name), sub_types(o.sub_types.begin(), o.sub_types.end()) {}
	type& operator = (const type& o) {name = o.name; sub_types.assign(o.sub_types.begin(),o.sub_types.end()); return *this;}

	std::unique_ptr<type> clone() const {
		auto ret = std::make_unique<type>();
		ret->name = name;
		ret->sub_types.assign(sub_types.begin(), sub_types.end());
		return ret;
	}

	std::string name;
	boost::ptr_vector<type> sub_types;
};

struct func_param {
	std::string name;
	type param_type;
};

struct function {
	std::string name;
	type return_type;
	std::optional<bool> is_mutable;
	std::optional<bool> is_static;
	std::vector<func_param> func_params;
	meta_parameters::parameter_set meta_params;
};

struct constructor_fnc {
	std::vector<func_param> func_params;
	meta_parameters::parameter_set meta_params;
};

struct enum_element {
	std::string name;
	std::string io;
};
struct enumeration {
	std::string name;
	std::vector<enum_element> elements;
	meta_parameters::parameter_set meta_params;

	bool gen_io=false;
	bool use_bitmask=false;
};

struct record_item {
	std::string name;
	type param_type;
	meta_parameters::parameter_set meta_params;
};

struct record {
	std::string name;
	std::vector<record_item> members;
	meta_parameters::parameter_set meta_params;
};

struct interface {
	std::string name;
	std::vector<function> mem_funcs;
	std::vector<constructor_fnc> constructors;
	meta_parameters::parameter_set meta_params;
	bool realization_in_client = false;
};

typedef std::variant<function,enumeration,record,interface> module_content;
enum class module_content_selector{function = 1 << 0, enumeration = 1 << 1, record = 1 << 2, interface = 1 << 3, all = ~0};

struct export_info {
	std::string name;
	module_content_selector type;
};
struct using_directive {
	std::string mod_name;
	bool is_system = false;
	std::vector<export_info> dest_items;

	bool is_from_system() const { return is_system; }
	bool is_from_other_module() const { return !is_from_system() && dest_items.empty(); }
	bool is_extra() const { return !is_from_system() && !dest_items.empty(); }
};
struct module {
	std::string name;
	std::vector<module_content> content;
	meta_parameters::parameter_set meta_params;
	std::vector<using_directive> imports;
	std::vector<export_info> exports;
	std::string file_name;
};

struct parsed_file {
	parsed_file() =default ;
	parsed_file(const std::vector<module>& m) : mods(m) {}

	std::vector<module> mods;
	std::vector<std::string> includes;
	std::string path;

	void push_back(module p);
	auto begin() {return mods.begin(); }
	auto end() {return mods.end(); }
	auto begin() const {return mods.begin(); }
	auto end() const {return mods.end(); }
};

} // namespace modegen::parser::interface

BOOST_FUSION_ADAPT_STRUCT( modegen::parser::interface::type, name, sub_types )
BOOST_FUSION_ADAPT_STRUCT( modegen::parser::interface::func_param, (modegen::parser::interface::type, param_type), (std::string, name) )
BOOST_FUSION_ADAPT_STRUCT( modegen::parser::interface::constructor_fnc, func_params, meta_params )
BOOST_FUSION_ADAPT_STRUCT( modegen::parser::interface::function
                           , (modegen::parser::interface::type, return_type)
                           , (std::string, name)
                           , (std::vector<modegen::parser::interface::func_param>, func_params)
                           , (modegen::parser::interface::meta_parameters::parameter_set, meta_params)
                           , (std::optional<bool>, is_mutable)
                           , (std::optional<bool>, is_static)
                          )
BOOST_FUSION_ADAPT_STRUCT( modegen::parser::interface::enum_element, name, io )
BOOST_FUSION_ADAPT_STRUCT( modegen::parser::interface::enumeration, name, elements, meta_params, gen_io, use_bitmask )
BOOST_FUSION_ADAPT_STRUCT( modegen::parser::interface::record_item, meta_params, param_type, name )
BOOST_FUSION_ADAPT_STRUCT( modegen::parser::interface::record, name, members, meta_params )
BOOST_FUSION_ADAPT_STRUCT( modegen::parser::interface::interface, name, mem_funcs, constructors, meta_params, realization_in_client )
BOOST_FUSION_ADAPT_STRUCT( modegen::parser::interface::using_directive, mod_name )
BOOST_FUSION_ADAPT_STRUCT( modegen::parser::interface::parsed_file, mods, includes )
BOOST_FUSION_ADAPT_STRUCT( modegen::parser::interface::module
                           , name
                           , content
                           , meta_params
                           , imports
                         )

