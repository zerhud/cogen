/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "cpp.hpp"

#include "errors.h"
#include "interface/naming.hpp"
#include "interface/to_json.hpp"
#include "interface/filter.hpp"
#include "interface/split_version.hpp"
#include "cpp/type_converter.hpp"
#include "parser/interface/loader.hpp"
#include "parser/interface/helpers.hpp"

using namespace std::literals;
namespace mg = modegen::generation;
namespace mi = modegen::parser::interface;

namespace modegen::generation {
struct json_extra_info : interface::to_json_aspect {
	void as_object(cppjson::value& jval, const mi::module& obj) override
	{
		jval["namespace"] = obj.name + "_v"s + get_version(obj).value("_"sv) ;
	}
};
} // modegen::generation

cppjson::value mg::cpp_generator::jsoned_data(const std::vector<parser::loader_ptr>& data_loaders, options_view opts) const
{
	using namespace modegen::generation::interface;
	using modegen::generation::interface::operator |;

	parser::interface::loader* ildr = nullptr;
	for(auto& dl:data_loaders) {
		ildr = dynamic_cast<parser::interface::loader*>(dl.get());
		if(ildr) break;
	}
	if(!ildr) throw errors::gen_error("cpp"s, "cannot generate cpp not from interface declaration or with wrong loader"s);

	cpp::type_converter tcvt;

	auto data = ildr->result();
	if(data.empty()) throw errors::no_data("cpp"s);

	filter::request freq;
	freq.mod_name = opts.part_data().get("filter.mod", ""s);
	freq.cnt_name = opts.part_data().get("filter.cnt", ""s);
	freq.sel = parser::interface::from_string(opts.part_data().get("filter.sel", ""s));

	cppjson::value jsoned =
	          data
	        | tcvt
	        | filter(freq)
	        | naming(opts.naming())
	        | split_version(!opts.target_data("cpp"sv).get("split_by_version", false))
	        | interface::to_json(std::make_unique<json_extra_info>())
	        ;

	auto incs = includes(tcvt.includes(), opts);
	for(std::size_t i=0;i<incs.size();++i) {
		jsoned["incs"][i]["n"] = incs[i].name;
		jsoned["incs"][i]["sys"] = incs[i].sys;
	}

	add_extra_info(opts, jsoned);

	return jsoned;
}

std::vector<mg::cpp_generator::inc_info> mg::cpp_generator::includes(const std::vector<std::string> sys, mg::options_view& opts) const
{
	std::vector<inc_info> ret;
	ret.reserve(sys.size());

	for(auto& s:sys) ret.emplace_back(std::move(s), true);
	for(auto& i:opts.part_data()) {
		if(i.first == "inc_sys"sv) {
			ret.emplace_back(i.second.get_value<std::string>(), true);
		}
	}
	for(auto& i:opts.part_data()) {
		if(i.first == "inc_part"sv) {
			ret.emplace_back(solve_part_include(i.second.get_value<std::string>(), opts), false);
		}
	}
	for(auto& i:opts.part_data()) {
		if(i.first == "inc_local"sv) {
			ret.emplace_back(i.second.get_value<std::string>(), false);
		}
	}

	return ret;
}

std::string mg::cpp_generator::solve_part_include(const std::string& part, mg::options_view& opts) const
{
	return opts.part_data(part).get<std::string>("output");
}

void mg::cpp_generator::add_extra_info(options_view& opts, cppjson::value& cdata) const
{
	add_extra_namespaces(opts, cdata);
	set_constructors_prefix(opts, cdata);
}

void mg::cpp_generator::add_extra_namespaces(mg::options_view& opts, cppjson::value& cdata) const
{
	auto nsopts = opts.all().get_child_optional("target.cpp");
	if(!nsopts) return;

	std::size_t i=0;
	for(auto& en:*nsopts) {
		if(en.first!="namespaces"sv) continue;
		cdata["namespaces"][i++] = en.second.get_value<std::string>();
	}
}

void mg::cpp_generator::set_constructors_prefix(mg::options_view& opts, cppjson::value& cdata) const
{
	using namespace modegen::generation::interface;

	auto ctor_pref = opts.target_data("cpp"sv).get_optional<std::string>("ctor_prefix");
	auto ptr_suf = opts.target_data("cpp"sv).get_optional<std::string>("ptr_suffix");

	name_conversion naming = from_string(opts.naming());
	if(naming==name_conversion::title_case) {
		cdata["ctor_prefix"] = ctor_pref ? *ctor_pref : "Create"s;
		cdata["ptr_suffix"] = ptr_suf ? *ptr_suf : "Ptr"s;
	} else if(naming==name_conversion::camel_case){
		cdata["ctor_prefix"] = ctor_pref ? *ctor_pref : "create"s;
		cdata["ptr_suffix"] = ptr_suf ? *ptr_suf : "Ptr"s;
	} else  {
		cdata["ctor_prefix"] = ctor_pref ? *ctor_pref : "create_"s;
		cdata["ptr_suffix"] = ptr_suf ? *ptr_suf : "_ptr"s;
	}
}

