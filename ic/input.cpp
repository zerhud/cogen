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

std::vector<const gen_utils::tree*> input::select(std::string_view id) const
{
	std::vector<const gen_utils::tree*> ret;
	ret.reserve(storage.size());
	for(auto& t:storage) if(t.data_id()==id) ret.emplace_back(&t);
	return ret;
}

std::vector<const gen_utils::tree*> input::all() const
{
	std::vector<const gen_utils::tree*> ret;
	ret.reserve(storage.size());
	for(auto& t:storage) ret.emplace_back(&t);
	return ret;
}
