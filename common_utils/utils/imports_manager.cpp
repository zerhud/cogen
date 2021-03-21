/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
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
        const std::pmr::string& part,
        const std::pmr::string& file,
        const input& data)
{
	return add(part, file, data);
}

imports_manager& imports_manager::add(
        const std::pmr::string& part,
        const std::pmr::string& file,
        const input& data)
{
	input_store.emplace_back(input_info{.file=file,.part=part,.data=&data});
	return *this;
}

void imports_manager::build()
{
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

std::pmr::vector<import_info> imports_manager::remove_own_part(
    const input& file_data, std::pmr::vector<import_info> src) const
{
	std::pmr::string cur_part;
	for(auto& [file, part, data]:input_store) if(data==&file_data) cur_part = part;
	for(auto& [file, part, data]:input_store) if(part==cur_part) {
		std::erase_if(src, [&file](const import_info& i){return i.file.name==file;});
	}
	return src;
}

std::pmr::vector<import_info> imports_manager::remove_itself(
    const input& file_data, std::pmr::vector<import_info> src) const
{
	for(auto& [file, part, data]:input_store) if(data==&file_data) {
		std::erase_if(src, [&file](const import_info& i){return i.file.name==file;});
	}
	return src;
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
	for(auto& [in_name, part_name, in]:input_store) {
		auto found = search_links(in->all(), cur->required_links(), in_name);
		for(auto& f:found) ret.emplace_back(std::move(f)).cfg = in->conf();
	}
	for(auto& r:ret) r.from = node_pointer{cur, &src};
	return ret;
}

std::pmr::vector<import_info> imports_manager::search_links(
      std::pmr::vector<const tree*> in_list
    , std::pmr::vector<gen_utils::name_t> required_links
    , const std::pmr::string& in_name) const
{
	std::pmr::vector<import_info> ret;
	for(auto&& in_tree:in_list) {
		for(auto& req: required_links) {
			auto found = search_links(*in_tree, req, in_name);
			ret.insert(ret.end(), found.begin(),found.end());
		}
	}
	return ret;
}

std::pmr::vector<import_info> imports_manager::search_links(
          const tree& in_tree
        , gen_utils::name_t req
        , const std::pmr::string& in_name) const
{
	std::pmr::vector<import_info> ret;
	std::pmr::vector<gen_utils::node_ptr> found_list = in_tree.search(req);
	for(auto& to:found_list) {
		auto mod = to->imports_modification();
		ret.emplace_back(import_info{
		                     .to={to, &in_tree},
		                     .file=mod ? *mod : import_file{false, in_name},
		                     .cond=std::pmr::string(to->link_condition())
		                 });
	}
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

imports_manager::incs_map_t imports_manager::required_includes(
	        const input& file_data) const
{
	std::pmr::map<std::pmr::string, std::pmr::vector<import_file>> ret;
	auto req = remove_own_part(file_data, required_for(file_data) | unique);
	for(auto& r:req) ret[r.cond].emplace_back(r.file);
	return ret;
}

imports_manager::incs_map_t imports_manager::required_includes_with_own(
	        const input& file_data) const
{
	std::pmr::map<std::pmr::string, std::pmr::vector<import_file>> ret;
	auto req = remove_itself(file_data, required_for(file_data) | unique);
	for(auto& r:req) ret[r.cond].emplace_back(r.file);
	return ret;
}

imports_manager::incs_map_t imports_manager::mapped_includes(
	        std::string_view tmpl,
	        const gen_utils::input& src) const
{
	gen_utils::map_to::result_inputs_t mapped;
	for(auto& [t,p,i]:input_store) if(i != &src) mapped[t]=*i;
	auto result = gen_utils::map_from()(mapped, tmpl, src);
	std::pmr::map<std::pmr::string, std::pmr::vector<import_file>> ret;
	for(auto& [k,v]:result) for(auto& f:v) ret[k].emplace_back(import_file{false, f});
	return ret;
}
