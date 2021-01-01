/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "imports_manager.hpp"
#include "input.hpp"

using mdg::ic::import_info;
using mdg::ic::imports_manager;

imports_manager& imports_manager::operator()(const std::pmr::string& file, const input& data)
{
	return add(file, data);
}

imports_manager& imports_manager::add(const std::pmr::string& file, const mdg::ic::input& data)
{
	all_input[file] = &data;
	return *this;
}

void imports_manager::build()
{
	scan_self_matched();
	scan_required_imports();
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

void imports_manager::scan_required_imports()
{
	for(auto& [from, fdata]:all_input) {
		for(auto& [to, todata]:all_input) {
			for(auto& ft:fdata->all()) {
				for(auto& tt:todata->all()) {
					scan_required_imports(*ft, *tt);
				}
			}
		}
	}
}

import_info* imports_manager::scan_required_imports(
        const gen_utils::tree& from, const gen_utils::tree& to)
{
	if(&from == &to) return nullptr;
	return nullptr;
}

std::pmr::vector<mdg::ic::import_info> imports_manager::required_for(
        const mdg::ic::input& file_data) const
{
	std::pmr::vector<mdg::ic::import_info> ret;
	for(auto& dt:file_data.all()) {
		auto dt_result = required_for_scan(*dt, dt->root());
		ret.insert(ret.end(), dt_result.begin(), dt_result.end());
	}
	return ret;
}

std::pmr::vector<import_info> imports_manager::required_for_scan(
    const gen_utils::tree& src, const gen_utils::data_node& par) const
{
	std::pmr::vector<import_info> ret;
	for(auto& child:src.children(par)) {
		auto child_result = required_for_links(src, child);
		ret.insert(ret.end(), child_result.begin(), child_result.end());
		child_result = required_for_scan(src, *child);
		ret.insert(ret.end(), child_result.begin(), child_result.end());
	}
	return ret;
}

std::pmr::vector<import_info> imports_manager::required_for_links(
    const gen_utils::tree& src, gen_utils::node_ptr cur) const
{
	std::pmr::vector<import_info> ret;
	for(auto& [in_name, in]:all_input) {
		for(auto& in_tree:in->all()) {
			if(in_tree == &src) continue;
			auto requests = cur->required_links();
			for(auto& req:requests) {
				auto r = in_tree->search(req);
				if(r) ret.emplace_back(import_info{{r, in_tree}, {cur, &src}, in_name, ""});
			}
		}
	}
	return ret;
}

std::pmr::vector<std::pmr::string> imports_manager::self_matched(const input& file_data) const
{
	std::pmr::vector<std::pmr::string> ret;
	auto pos = matched.find(&file_data);
	if(pos!=matched.end()) for(auto& f:pos->second) ret.emplace_back(f);
	return ret;
}
