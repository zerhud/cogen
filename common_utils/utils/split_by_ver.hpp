/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
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
	std::optional<gen_utils::tree> copy_for_next(const tree& src) const ; 
};

} // namespace gen_utils
