/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "module.hpp"

#include "interface/common.hpp"
#include "interface/filter.hpp"
#include "interface/naming.hpp"
#include "interface/split_version.hpp"
#include "options.hpp"

#include "parser/interface/helpers.hpp"

#include "pg/exceptions.hpp"

namespace mp = modegen::pg;
namespace mpp = modegen::pg::palgos;
namespace mpi = modegen::parser::interface;
using namespace std::literals;

mpp::interface_conf_algos::interface_conf_algos(const std::vector<modegen::parser::loader_ptr>& ldrs)
{
	std::shared_ptr<parser::interface::loader> mldr;
	for(auto ldr:ldrs) if((mldr = std::dynamic_pointer_cast<mpi::loader>(ldr))) break;
	if(!mldr) throw errors::error("cannot create module algo without module loader");

	mods_ = mldr->result();
}

mp::configuration_algos::mapped_data mpp::interface_conf_algos::map_to(mp::configuration_algos::mapped_data md)
{
	mapped_.clear();
	mapped_data ret;

	for(auto& cur_d:md) {
		auto mapped = inner_map(cur_d.first);
		for(auto& item:mapped) {
			ret[item.first].emplace_back(item.second);
			for(auto& ii:item.second) mapped_[item.first].emplace_back(ii);
		}
	}

	return ret;
}

std::map<std::string,std::any> mpp::interface_conf_algos::map_to_str(const std::string& tmpl)
{
	tmpl_ = tmpl;
	mapped_ = inner_map(tmpl);

	std::map<std::string,std::any> ret;
	for(auto& [key,val]:mapped_) ret[key]=val;
	return ret;
}

std::map<std::string, std::vector<std::string>> mpp::interface_conf_algos::map_from(const std::string& tmpl)
{
	auto mapped = inner_map(tmpl);
	std::map<std::string, std::vector<std::string>> ret;

	for(auto& [mtmpl, mdata]:mapped) {
		std::vector<std::string>& out_set = ret[mtmpl];
		for(auto& mod:mdata) out_set.emplace_back(require_data(mod));
	}

	return ret;
}

void mpp::interface_conf_algos::set_filter(const options::part_view& pinfo)
{
	using namespace mpp::interface;
	using namespace options;

	filter::request req;
	std::string versioning = pinfo.get_opt<std::string>(options::template_option::versioning).value_or("merge"s);
	mods_
		| split_version(versioning != "split"sv)
		| filter(req)
		| naming(from_string(pinfo.get<std::string>(part_option::naming)))
		;
}

std::vector<std::string> mpp::interface_conf_algos::map(const std::string& tmpl) const
{
	using namespace std::literals;

	std::vector<std::string> ret;

	for(auto& mod:mods_) {
		std::string& cur = ret.emplace_back(tmpl);
		auto ver = mpi::get_version(mod);
		replace(cur, "$mod"s, mod.name);
		replace(cur, "$vm"s, std::to_string(ver.major_v));
		replace(cur, "$vi"s, std::to_string(ver.minor_v));
	}

	return ret;
}

bool mpp::interface_conf_algos::replace(std::string& tmpl, const std::string& var_name, const std::string& value) const
{
	auto pos = tmpl.find(var_name);
	bool found = pos!=std::string::npos;
	if(found) tmpl.replace(pos, var_name.size(), value);
	return found;
}

std::map<std::string, std::vector<mpi::module>> mpp::interface_conf_algos::inner_map(const std::string& tmpl) const
{
	std::map<std::string, std::vector<mpi::module>> ret;

	for(auto& mod:mods_) {
		std::string cur = tmpl;

		const bool rmod = replace(cur, "$mod"s, mod.name);
		if(!rmod) {
			ret[cur] = mods_;
			break;
		}

		auto ver = mpi::get_version(mod);
		replace(cur, "$va"s, std::to_string(ver.major_v));
		replace(cur, "$vi"s, std::to_string(ver.minor_v));
		ret[cur].emplace_back(mod);
	}

	return ret;
}

std::string mpp::interface_conf_algos::require_data(const mpi::module& mod) const
{
	using namespace modegen::parser::interface;
	for(auto& [file,imods]:mapped_) for(auto& imod:imods) if(imod==mod) return file;
	throw errors::error("cannot find module "s+mod.name+" in mapped to files");
}

std::vector<modegen::parser::interface::module> mpp::interface_conf_algos::mods() const
{
	return mods_;
}

