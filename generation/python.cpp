/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "python.hpp"

namespace mg = modegen::generation;

nlohmann::json mg::python::jsoned_data(const std::vector<parser::loader_ptr>& data_loaders, options::view opts) const
{
	nlohmann::json ret;
	ret["generator"] = "python";
	return ret;
}

