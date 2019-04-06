/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "python.hpp"
#include "output_info.hpp"
#include "part_descriptor.hpp"

namespace mg = modegen::generation;

nlohmann::json mg::python::jsoned_data(const mg::output_info& outputs) const
{
	TODO("place code here");
	nlohmann::json ret;
	ret["generator"] = "python";

	auto& cur_part = *outputs.current_part();
	for(auto& mod:cur_part.idl_input()) ret["mod_name"].push_back(mod.name);
	return ret;
}
