/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "map_from.hpp"

#include <set>

using gen_utils::map_from;


//typedef std::pmr::map<std::pmr::string, tree> result_t;

map_from::result_t map_from::operator()(
    const map_to::result_t& mapped,
    std::string_view tmpl,
    const tree& src)
{
	result_t ret;
	for(auto& [sn,sd]:map_to()(std::pmr::string(tmpl), src)) {
		for(auto&[mn,md]:mapped) {
			if(md.contains(sd) == tree_compare_result::total) {
				ret[sn].emplace_back(mn);
				continue;
			}
		}
	}
	return ret;
}
