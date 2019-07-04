/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "part_manager.hpp"

namespace mpg = modegen::pg;

std::size_t mpg::part_manager::count() const
{
	return store.size();
}

void mpg::part_manager::add(mpg::part_descriptor_ptr p)
{
	store.emplace_back(std::move(p));
}

