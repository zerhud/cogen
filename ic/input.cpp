/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "input.hpp"

using namespace std::literals;
using modegen::ic::input;

void input::add(input&& other)
{
	for(gen_utils::tree& oc:other.storage)
		add(std::move(oc));
}

void input::add(gen_utils::tree data)
{
	storage.emplace_back(std::move(data));
}

std::pmr::vector<const gen_utils::tree*> input::select(std::string_view id) const
{
	std::pmr::vector<const gen_utils::tree*> ret;
	ret.reserve(storage.size());
	for(auto& t:storage) if(t.data_id()==id) ret.emplace_back(&t);
	return ret;
}

std::pmr::vector<const gen_utils::tree*> input::all() const
{
	std::pmr::vector<const gen_utils::tree*> ret;
	ret.reserve(storage.size());
	for(auto& t:storage) ret.emplace_back(&t);
	return ret;
}

gen_utils::tree_compare_result input::match_with(const input& other) const
{
	using gen_utils::tree_compare_result;
	tree_compare_result result = tree_compare_result::none;
	if(storage.empty() && other.storage.empty())
		return tree_compare_result::total;
	for(auto& ot:other.storage) {
		for(auto& st:storage) {
			auto r = st.contains(ot);
			if( r == tree_compare_result::partial )
				return tree_compare_result::partial;
			if( r == tree_compare_result::total )
				result = tree_compare_result::total;
			else if( r== tree_compare_result::only_root )
				result = tree_compare_result::only_root;
		}
	}
	if(result == tree_compare_result::total
	        && other.storage.size() != storage.size())
		result = tree_compare_result::partial;
	return result;
}
