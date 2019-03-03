/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "cpp.hpp"

#include <boost/property_tree/json_parser.hpp>

#include "errors.h"
#include "interface/naming.hpp"
#include "interface/to_json.hpp"
#include "interface/filter.hpp"
#include "interface/split_version.hpp"
#include "cpp/type_converter.hpp"
#include "parser/interface/helpers.hpp"
#include "parser/interface/loader.hpp"
#include "parser/data_tree/loader.hpp"

using namespace std::literals;
namespace mg = modegen::generation;
namespace mi = modegen::parser::interface;

namespace modegen::generation {
struct json_extra_info : interface::to_json_aspect {
	void as_object(nlohmann::json& jval, const mi::module& obj) override
	{
		jval["namespace"] = obj.name + "_v"s + get_version(obj).value("_"sv) ;
	}
};

static auto get_loaders(const std::vector<parser::loader_ptr>& data_loaders)
{
	parser::interface::loader* ildr = nullptr;
	parser::data_tree::loader* dldr = nullptr;

	for(auto& dl:data_loaders) {
		auto* i_ildr = dynamic_cast<parser::interface::loader*>(dl.get());
		if(i_ildr) ildr = i_ildr;

		auto* i_dldr = dynamic_cast<parser::data_tree::loader*>(dl.get());
		if(i_dldr) dldr = i_dldr;
	}

	if(!ildr) throw errors::gen_error("cpp"s, "cannot generate cpp not from interface declaration or with wrong loader"s);

	return std::make_tuple(ildr, dldr);
}

static nlohmann::json convert(parser::data_tree::loader* dldr)
{
	nlohmann::json ret;
	if(dldr) {
		const boost::property_tree::ptree data = dldr->boost_ptree();
		std::stringstream cvt;
		boost::property_tree::write_json(cvt, data);
		cvt >> ret;
	}
	return ret;
}
} // modegen::generation

nlohmann::json mg::cpp_generator::jsoned_data(const std::vector<parser::loader_ptr>& data_loaders, options::view opts) const
{
	using namespace modegen::generation::interface;
	using modegen::generation::interface::operator |;

	auto [ildr, dldr] = get_loaders(data_loaders);
	assert(ildr);

	cpp::type_converter tcvt;

	auto data = ildr->result();
	if(data.empty()) throw errors::no_data("cpp"s);

	filter::request freq;
	//options::filter_view fw(opts.container());
	//std::vector<filter::request> freq_list;
	//for(auto f:fw.in_part()) {
	//}

	//freq.mod_name = opts.part_data().get("filter.mod", ""s);
	//freq.cnt_name = opts.part_data().get("filter.cnt", ""s);
	//freq.sel = parser::interface::from_string(opts.part_data().get("filter.sel", ""s));

	std::string versioning = opts.get_opt<std::string>(options::template_option::versioning, ""s, "cpp"s).value_or(""s);
	nlohmann::json jsoned =
	          data
	        | tcvt
	        | filter(freq)
	        | naming(opts.get_opt<std::string>(options::part_option::naming).value_or(""s))
	        | split_version(versioning == "split"sv)
	        | interface::to_json(std::make_unique<json_extra_info>())
	        ;

	auto incs = includes(tcvt.includes(), opts);
	for(std::size_t i=0;i<incs.size();++i) {
		jsoned["incs"][i]["n"] = incs[i].name;
		jsoned["incs"][i]["sys"] = incs[i].sys;
	}

	add_extra_info(opts, jsoned);
	jsoned["extra_data"] = convert(dldr);

	return jsoned;
}

std::vector<mg::cpp_generator::inc_info> mg::cpp_generator::includes(const std::vector<std::string> sys, mg::options::view& opts) const
{
	std::vector<inc_info> ret;
	ret.reserve(sys.size());

	auto part_data = opts.get_subset(options::subsetts::part_data);

	for(auto& s:sys) ret.emplace_back(std::move(s), true);
	for(auto& i:part_data) {
		if(i.first == "inc_sys"sv) {
			ret.emplace_back(i.second.get_value<std::string>(), true);
		}
	}
	for(auto& i:part_data) {
		if(i.first == "inc_part"sv) {
			ret.emplace_back(solve_part_include(i.second.get_value<std::string>(), opts), false);
		}
	}
	for(auto& i:part_data) {
		if(i.first == "inc_local"sv) {
			ret.emplace_back(i.second.get_value<std::string>(), false);
		}
	}

	return ret;
}

std::string mg::cpp_generator::solve_part_include(const std::string& part, mg::options::view& opts) const
{
	return opts.get<std::string>(options::part_option::output, part);
}

void mg::cpp_generator::add_extra_info(options::view& opts, nlohmann::json& cdata) const
{
	add_extra_namespaces(opts, cdata);
	set_constructors_prefix(opts, cdata);
}

void mg::cpp_generator::add_extra_namespaces(mg::options::view& opts, nlohmann::json& cdata) const
{
	auto nsopts = opts.get_subset(options::subsetts::file_generator, "cpp"s, ""s);

	std::size_t i=0;
	for(auto& en:nsopts) {
		if(en.first!="namespaces"sv) continue;
		cdata["namespaces"][i++] = en.second.get_value<std::string>();
	}
}

void mg::cpp_generator::set_constructors_prefix(mg::options::view& opts, nlohmann::json& cdata) const
{
	using namespace modegen::generation::interface;

	auto nsopts = opts.get_subset(options::subsetts::file_generator, "cpp"s, ""s);
	auto ctor_pref = nsopts.get_optional<std::string>("ctor_prefix");
	auto ptr_suf = nsopts.get_optional<std::string>("ptr_suffix");

	name_conversion naming = from_string(opts.get_opt<std::string>(options::part_option::naming).value_or(""s));
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

