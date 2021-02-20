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

map_from::result_t map_from::operator()(
            const map_to::result_inputs_t& mapped,
            std::string_view tmpl,
            const input& src)
{
	result_t ret;
	auto mr = map_to()(std::pmr::string(tmpl), src);
	for(auto& [sn,sd]:mr) {
		for(auto& [mn,md]:mapped) {
			if(md.match_with(sd) == tree_compare_result::total) {
				ret[sn].emplace_back(mn);
				continue;
			}
		}
	}
	return ret;
}
