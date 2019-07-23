/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "cpp.hpp"

#include "part_descriptor.hpp"
#include "part_algos/module.hpp"
#include "part_algos/interface/common.hpp"
#include "part_algos/interface/to_json.hpp"
#include "cpp/type_converter.hpp"

#include "parser/interface/helpers.hpp"

#include "exceptions.hpp"

namespace mpg = modegen::pg;
namespace mgo = modegen::pg::outputs;
namespace mpi = modegen::parser::interface;
namespace mai = modegen::generation::interface;
using namespace std::literals;

mgo::cpp::cpp(provider_const_ptr p, FS::path o, const part_descriptor& part)
	: out_file_(std::move(o))
	, imngs_(part.input_managers())
	, opts_(part.opts())
{
}

mpg::output_lang mgo::cpp::lang() const
{
	return output_lang::cpp;
}

void mgo::cpp::override_setts(boost::property_tree::ptree s)
{
}

nlohmann::json mgo::cpp::data() const
{
	using generation::interface::operator |;

	struct json_extra_info : generation::interface::to_json_aspect {
		void as_object(nlohmann::json& jval, const mpi::module& obj) override
		{
			jval["namespace"] = obj.name + "_v"s + get_version(obj).value("_"sv) ;
		}
	};

	std::shared_ptr<mpg::palgos::module_algos> malg;
	for(auto&i:imngs_) if(malg=std::dynamic_pointer_cast<mpg::palgos::module_algos>(i);malg) break;
	assert(malg);

	mai::to_json jsoner(std::make_unique<json_extra_info>());
	generation::cpp::type_converter tcvt;

	auto mods = malg->mods();
	nlohmann::json jsoned = mods | tcvt | jsoner;

	auto incs = includes(tcvt.includes());
	for(std::size_t i=0;i<incs.size();++i) {
		jsoned["incs"][i]["n"] = incs[i].name;
		jsoned["incs"][i]["sys"] = incs[i].sys;
	}

	add_extra_namespaces(jsoned);
	TODO("part must call it?");
	set_constructors_prefix(jsoned);

	return jsoned;
}

FS::path mgo::cpp::file() const
{
	return out_file_;
}

std::vector<mgo::cpp::inc_info> mgo::cpp::includes(const std::vector<std::string> sys) const
{
	std::vector<inc_info> ret;
	ret.reserve(sys.size());

	auto part_data = opts_.get_subset(options::subsetts::part_data);

	for(auto& s:sys) ret.emplace_back(std::move(s), true);
	for(auto& i:part_data) {
		if(i.first == "inc_sys"sv) {
			ret.emplace_back(i.second.get_value<std::string>(), true);
		}
	}
	for(auto& i:part_data) {
		if(i.first == "inc_part"sv) {
			TODO("need to include other part, get output and include it");
			//ret.emplace_back(solve_part_include(i.second.get_value<std::string>(), opts), false);
		}
	}
	for(auto& i:part_data) {
		if(i.first == "inc_local"sv) {
			ret.emplace_back(i.second.get_value<std::string>(), false);
		}
	}

	return ret;
}

void mgo::cpp::add_extra_namespaces(nlohmann::json& cdata) const
{
	auto nsopts = opts_.get_subset(options::subsetts::file_generator, "cpp"s, ""s);

	std::size_t i=0;
	for(auto& en:nsopts) {
		if(en.first!="namespaces"sv) continue;
		cdata["namespaces"][i++] = en.second.get_value<std::string>();
	}
}


void mgo::cpp::set_constructors_prefix(nlohmann::json& cdata) const
{
	using namespace modegen::generation::interface;

	auto nsopts = opts_.get_subset(options::subsetts::file_generator, "cpp"s, ""s);
	auto ctor_pref = nsopts.get_optional<std::string>("ctor_prefix");
	auto ptr_suf = nsopts.get_optional<std::string>("ptr_suffix");

	name_conversion naming = from_string(opts_.get_opt<std::string>(options::part_option::naming).value_or(""s));
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

