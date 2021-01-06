/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "imports_manager.hpp"
#include "input.hpp"

using gen_utils::import_info;
using gen_utils::imports_manager;

imports_manager& imports_manager::operator()(const std::pmr::string& file, const input& data)
{
	return add(file, data);
}

imports_manager& imports_manager::add(const std::pmr::string& file, const input& data)
{
	all_input[file] = &data;
	return *this;
}

void imports_manager::build()
{
	matched.clear();
	scan_self_matched();
}

void imports_manager::scan_self_matched()
{
	using tcr = tree_compare_result;
	for(auto& [from, fd]:all_input) {
		for(auto& [to, td]:all_input) {
			if(from == to) continue;
			tcr r = fd->match_with(*td);
			if(r==tcr::total || r==tcr::partial)
				matched[fd].emplace_back(to);
		}
	}
}

std::pmr::vector<import_info> imports_manager::required_for(
        const input& file_data) const
{
	std::pmr::vector<import_info> ret;
	for(auto& dt:file_data.all()) {
		auto dt_result = required_for_scan(*dt, dt->root());
		ret.insert(ret.end(), dt_result.begin(), dt_result.end());
	}
	return ret;
}

std::pmr::vector<import_info> imports_manager::required_for(
        const tree& file_data) const
{
	return required_for_scan(file_data, file_data.root());
}

std::pmr::vector<import_info> imports_manager::required_for_scan(
    const tree& src, const data_node& par) const
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
    const tree& src, node_ptr cur) const
{
	std::pmr::vector<import_info> ret;
	for(auto& [in_name, in]:all_input) {
		for(auto& in_tree:in->all()) {
			if(in_tree == &src) continue;
			auto requests = cur->required_links();
			for(auto& req:requests) {
				auto found_list = in_tree->search(req);
				for(auto& to:found_list)
					ret.emplace_back(
					            import_info{
					                {to, in_tree},
					                {cur, &src},
					                {false, "in_name"},
					                in_name,
					                std::pmr::string(to->link_condition())});
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
