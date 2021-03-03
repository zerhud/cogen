/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "input.hpp"

using namespace std::literals;
using gen_utils::input;

gen_utils::input& input::add(input&& other)
{
	for(gen_utils::tree& oc:other.storage)
		add(std::move(oc));
	return *this;
}

gen_utils::input& input::add(tree data)
{
	storage.emplace_back(std::move(data));
	return *this;
}

std::pmr::vector<const gen_utils::tree*> input::select(std::string_view id) const
{
	std::pmr::vector<const tree*> ret;
	ret.reserve(storage.size());
	for(auto& t:storage) if(t.data_id()==id) ret.emplace_back(&t);
	return ret;
}

bool input::contains(const tree* data) const
{
	auto list = select(data->data_id());
	return std::ranges::find(list, data) != list.end();
}

std::pmr::vector<const gen_utils::tree*> input::all() const
{
	std::pmr::vector<const tree*> ret;
	ret.reserve(storage.size());
	for(auto& t:storage) ret.emplace_back(&t);
	return ret;
}

gen_utils::tree_compare_result input::match_with(const input& other) const
{
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
	return result;
}

gen_utils::compilation_config& input::conf()
{
	return cur_conf;
}

const gen_utils::compilation_config& input::conf() const
{
	return cur_conf;
}

input input::modify(const input::mod_fnc& mod) const
{
	input ret;
	ret.cur_conf = cur_conf;
	for(auto &t:storage) {
		auto mr = mod(t);
		for(auto& r:mr)
			ret.storage.emplace_back(std::move(r));
	}
	return ret;
}
