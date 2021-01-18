/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "split_by_ver.hpp"

using gen_utils::split_by_ver;

std::pmr::vector<gen_utils::tree> split_by_ver::operator()(tree src)
{
	return {src};
}
