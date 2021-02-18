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

std::pmr::vector<std::pmr::string> map_from::operator()(
    const map_to::result_t& mapped, std::string_view tmpl, const tree& src)
{
	std::pmr::set<std::pmr::string> acc;
	auto mr = map_to()(std::pmr::string(tmpl), src);
	for(auto& [_,sd]:mr){
		for(auto&[mn,md]:mapped) {
			//auto mr = md.contains(sd);
			auto mr = sd.contains(md);
			if(mr == tree_compare_result::partial)
				acc.insert(mn);
			else if(mr == tree_compare_result::total)
				return {mn};
		}
	}
	std::pmr::vector<std::pmr::string> ret;
	ret.reserve(acc.size());
	for(auto& a:acc) ret.emplace_back(a);
	return ret;
}
