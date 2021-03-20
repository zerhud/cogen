/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "map_from.hpp"

#include <set>

using gen_utils::map_from;
using tcr = gen_utils::tree_compare_result;

map_from::result_t map_from::operator()(
    const map_to::result_t& mapped,
    std::string_view tmpl,
    const tree& src)
{
	result_t ret;
	for(auto& [sn,sd]:map_to()(std::pmr::string(tmpl), src)) {
		for(auto&[mn,md]:mapped) {
			if(sd.contains(md) == tcr::total) {
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
	for(auto& [sn,sd]:map_to()(std::pmr::string(tmpl), src)) {
		for(auto& [mn,md]:mapped) {
			if(matched(md, sd)) {
				ret[sn].emplace_back(mn);
				continue;
			}
		}
	}
	return ret;
}

bool map_from::matched(
        const gen_utils::input& gotten,
        const gen_utils::input& mapped) const
{
	for(auto& src_tree:mapped.all()) {
		if(!has(gotten, *src_tree)) continue;
		if(!contains(*src_tree, gotten)) return false;
	}
	return true;
}

bool map_from::has(const input& where, const tree& what) const
{
	for(auto& t:where.all()) {
		tcr r = t->contains(what);
		if(r != tcr::none && r != tcr::not_comparable) return true;
	}
	return false;
}

bool map_from::contains(const tree& what, const input& where) const
{
	for(auto t:where.all())
		if(what.contains(*t) == tcr::total) return true;
	return false;
}

bool map_from::contains(const gen_utils::input& where, const gen_utils::tree& what) const
{
	for(auto t:where.all())
		if(t->contains(what) == tcr::total) return true;
	return false;
}
