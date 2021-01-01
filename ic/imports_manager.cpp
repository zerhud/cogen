/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "imports_manager.hpp"
#include "input.hpp"

using mdg::ic::imports_manager;

imports_manager& imports_manager::add(const std::pmr::string& file, const mdg::ic::input& data)
{
	all_input[file] = &data;
	return *this;
}

void imports_manager::build()
{
	scan_self_matched();
}

void imports_manager::scan_self_matched()
{
	using tcr = gen_utils::tree_compare_result;
	for(auto& [from, fd]:all_input) {
		for(auto& [to, td]:all_input) {
			if(from == to) continue;
			tcr r = fd->match_with(*td);
			if(r==tcr::total || r==tcr::partial)
				matched[fd].emplace_back(to);
		}
	}
}

std::pmr::vector<mdg::ic::import_info> imports_manager::result_for(
        const mdg::ic::input& file_data) const
{
	return {};
}

std::pmr::vector<std::pmr::string> imports_manager::self_matched(const input& file_data) const
{
	std::pmr::vector<std::pmr::string> ret;
	auto pos = matched.find(&file_data);
	if(pos!=matched.end()) for(auto& f:pos->second) ret.emplace_back(f);
	return ret;
}
