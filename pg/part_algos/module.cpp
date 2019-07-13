/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "module.hpp"
#include "parser/interface/helpers.hpp"
#include "pg/exceptions.hpp"

namespace mpp = modegen::pg::palgos;
namespace mpi = modegen::parser::interface;

mpp::module_algos::module_algos(const std::vector<modegen::parser::loader_ptr>& ldrs)
{
	for(auto ldr:ldrs) if(mldr_ = std::dynamic_pointer_cast<mpi::loader>(ldr)) break;
	if(!mldr_) throw errors::error("cannot create module algo without module loader");
}

void mpp::module_algos::set_filter(boost::property_tree::ptree fdata)
{
	//TODO: support fliter
}

std::vector<std::string> mpp::module_algos::map(const std::string& tmpl) const
{
	assert(mldr_);
	using namespace std::literals;

	std::vector<std::string> ret;

	auto mods = mldr_->result();

	for(auto& mod:mods) {
		std::string& cur = ret.emplace_back(tmpl);
		auto ver = mpi::get_version(mod);
		replace(cur, "$mod"s, mod.name);
		replace(cur, "$va"s, std::to_string(ver.major_v));
		replace(cur, "$vi"s, std::to_string(ver.minor_v));
	}

	return ret;
}

bool mpp::module_algos::replace(std::string& tmpl, const std::string& var_name, const std::string& value) const
{
	auto pos = tmpl.find(var_name);
	bool found = pos!=std::string::npos;
	if(found) tmpl.replace(pos, var_name.size(), value);
	return found;
}

