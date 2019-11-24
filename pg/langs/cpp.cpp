/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "cpp.hpp"

#include "output_configurator.hpp"
#include "part_manager.hpp"
#include "part_algos/module.hpp"
#include "part_algos/interface/common.hpp"
#include "part_algos/interface/to_json.hpp"
#include "cpp/type_converter.hpp"

#include "parser/interface/helpers.hpp"

#include "exceptions.hpp"

namespace mpg = modegen::pg;
namespace mgo = modegen::pg::outputs;
namespace mpi = modegen::parser::interface;
namespace mai = modegen::pg::palgos::interface;
namespace mcpp = modegen::pg::cpp;
using namespace std::literals;

mgo::cpp::cpp(FS::path o, std::vector<std::any> data)
    : out_file_(std::move(o))
    , input_(std::move(data))
{
}

mpg::output_lang mgo::cpp::lang() const
{
	return output_lang::cpp;
}

void mgo::cpp::setts(const mpg::options::part_view& s)
{
	opts_ = s;
}

nlohmann::json mgo::cpp::data(const part_manager& pman) const
{
	using mpg::palgos::operator |;

	struct json_extra_info : palgos::interface::to_json_aspect {
		void as_object(nlohmann::json& jval, const mpi::module& obj) override
		{
			jval["namespace"] = obj.name + "_v"s + get_version(obj).value("_"sv) ;
		}
	};

	std::vector<mpi::module> mods;
	for(auto& id:input_) {
		if(typeid(mods)==id.type()) mods = std::any_cast<std::vector<mpi::module>>(id);
	}

	mai::to_json jsoner(std::make_unique<json_extra_info>());
	mcpp::type_converter tcvt;

	nlohmann::json jsoned = mods | tcvt | jsoner;

	auto incs = includes(tcvt.includes(), pman);
	for(std::size_t i=0;i<incs.size();++i) {
		jsoned["incs"][i]["n"] = incs[i].name;
		jsoned["incs"][i]["sys"] = incs[i].sys;
	}

	add_extra_namespaces(jsoned);
	set_constructors_prefix(jsoned);

	return jsoned;
}

FS::path mgo::cpp::file() const
{
	return out_file_;
}

std::vector<mgo::cpp::inc_info> mgo::cpp::solve_part_includes(const std::string& pname, const part_manager& pman) const
{
	part_descriptor_ptr inc_part = pman.require(pname);
	auto out_list = inc_part->outputs();

	std::vector<inc_info> ret;
	for(auto& out:out_list) {
		inc_info& ii = ret.emplace_back(out->file(), false);
		ii.is_cpp = out->lang() == output_lang::cpp;
	}
	return ret;
}

std::vector<mgo::cpp::inc_info> mgo::cpp::includes(const std::vector<std::string> sys, const part_manager& pman) const
{
	std::vector<inc_info> ret;
	ret.reserve(sys.size());

	auto part_data = opts_->get_subset(options::subsetts::part_data);

	for(auto& s:sys) ret.emplace_back(std::move(s), true);
	for(auto& i:part_data) {
		if(i.first == "inc_sys"sv) {
			ret.emplace_back(i.second.get_value<std::string>(), true);
		}
	}
	for(auto& i:part_data) {
		if(i.first == "inc_part"sv) {
			auto part_includes = solve_part_includes(i.second.get_value<std::string>(), pman);
			for(auto& pi:part_includes) ret.emplace_back(std::move(pi));
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
	auto nsopts = opts_->get_subset(options::subsetts::file_generator, "cpp"s, ""s);

	std::size_t i=0;
	for(auto& en:nsopts) {
		if(en.first!="namespaces"sv) continue;
		cdata["namespaces"][i++] = en.second.get_value<std::string>();
	}
}

// other languages may not have pointers and constructors can be with same name
// cause of it we place this function here insteed of part logic
void mgo::cpp::set_constructors_prefix(nlohmann::json& cdata) const
{
	auto nsopts = opts_->get_subset(options::subsetts::file_generator, "cpp"s, ""s);
	auto ctor_pref = nsopts.get_optional<std::string>("ctor_prefix");
	auto ptr_suf = nsopts.get_optional<std::string>("ptr_suffix");

	name_conversion naming = from_string(opts_->get_opt<std::string>(options::part_option::naming).value_or(""s));
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

