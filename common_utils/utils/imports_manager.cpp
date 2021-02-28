/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "imports_manager.hpp"

#include "input.hpp"
#include "map_from.hpp"

#include <algorithm>
#include <ranges>

using gen_utils::import_info;
using gen_utils::imports_manager;

template< typename C, typename F>
auto operator | (C && c, F f)
{
	return f(c);
}

imports_manager& imports_manager::operator()(
        const std::pmr::string& file, const input& data)
{
	return add(file, data);
}

imports_manager& imports_manager::add(
        const std::pmr::string& file, const input& data)
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

std::pmr::vector<import_info> imports_manager::required_for_incs(
        const input& file_data) const
{
	auto ret = required_for(file_data) | unique;
	for(auto& [file, data]:all_input) if(data==&file_data) {
		auto pos = std::find_if(
				ret.begin(),ret.end(),
				[&file](auto& i){return i.file.name==file;});
		if(pos!=ret.end()) ret.erase(pos);
	}
	return ret;
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
			auto requests = cur->required_links();
			for(auto& req:requests) {
				auto found_list = in_tree->search(req);
				for(auto& to:found_list) {
					auto mod = to->imports_modification();
					import_file link{ false, in_name };
					if(mod) link = *mod;
					ret.emplace_back(
					            import_info{
					                {to, in_tree},
					                {cur, &src},
					                link,
					                std::pmr::string(to->link_condition()),
							in->conf()});
				}
			}
		}
	}
	return ret;
}

std::pmr::vector<std::pmr::string> imports_manager::self_matched(
        const input& file_data) const
{
	std::pmr::vector<std::pmr::string> ret;
	auto pos = matched.find(&file_data);
	if(pos!=matched.end()) for(auto& f:pos->second) ret.emplace_back(f);
	return ret;
}

std::pmr::vector<import_info> imports_manager::unique(
            std::pmr::vector<import_info> src)
{
	auto less = [](auto& l, auto& r) {
		return std::tie(l.cond, l.file) < std::tie(r.cond, r.file); };
	auto comparater = [](auto& l, auto& r){
		return l.cond == r.cond && l.file == r.file; };
	std::sort(src.begin(), src.end(), less);
	auto [f,t] = std::ranges::unique(src, comparater);
	src.erase(f,t);
	return src;
}

std::pmr::map<std::pmr::string, std::pmr::vector<gen_utils::import_file>>
	imports_manager::all_includes(const input& file_data) const
{
	std::pmr::map<std::pmr::string, std::pmr::vector<import_file>> ret;
	auto self = self_matched(file_data);
	for(auto& s:self) ret["self"].emplace_back(import_file{false, s});
	auto req = required_for_incs(file_data);
	for(auto& r:req) ret[r.cond].emplace_back(r.file);
	return ret;
}

std::pmr::map<std::pmr::string, std::pmr::vector<gen_utils::import_file>> imports_manager::map_from(
	        std::string_view tmpl,
	        const gen_utils::tree& src) const
{
	gen_utils::map_to::result_inputs_t mapped;
	for(auto& [t,i]:all_input) mapped[t]=*i;
	input isrc;
	isrc.add(src);
	auto result = gen_utils::map_from()(mapped, tmpl, isrc);
	std::pmr::map<std::pmr::string, std::pmr::vector<import_file>> ret;
	for(auto& [k,v]:result) for(auto& f:v) ret[k].emplace_back(import_file{false, f});
	return ret;
}
