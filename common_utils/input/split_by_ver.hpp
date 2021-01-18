/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <map>
#include <string>
#include <memory_resource>

#include "tree.hpp"

namespace gen_utils {

class split_by_ver {
public:
	std::pmr::vector<tree> operator()(tree src);
private:
};

} // namespace gen_utils
