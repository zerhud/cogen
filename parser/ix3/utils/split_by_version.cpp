/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "split_by_version.hpp"

using namespace ix3::utils;
using ix3::ast::meta::version;

split_by_version::split_by_version(bool dry_run) : dry(dry_run) {}

void split_by_version::eval_module(ast::module& mod)
{
	if(dry) result.emplace_back(std::move(mod));
	else trav_module(mod, trav_direction::child_first);
}

std::vector<ix3::ast::module>& split_by_version::extract_result()
{
	finished_result.clear();
	for(auto& r:result) for(auto& m:r.mods()) finished_result.emplace_back(m);
	return finished_result;
}

void split_by_version::on_obj(ast::module& obj)
{
	cpy_by_ver_t* current = nullptr;
	for(auto& r:result) {
		if(r.name()==obj.name) {
			current = &r;
			break;
		}
	}

	if(current) current->add(obj);
	else current = &result.emplace_back(obj);

	split_content(obj);

	auto adder = [this,current](auto& obj) {
		using ast::meta::version;
		auto obj_ver = ast::get<version>(obj.meta_params);
		current->replace_after(obj, obj_ver); };
	for(auto& cnt:obj.content) boost::apply_visitor(adder, cnt.var);
}

void split_by_version::split_content(ast::module& mod) const
{
	decltype(mod.content) for_add;
	auto remove_begin = std::remove_if(
		mod.content.begin(), mod.content.end(),
		[this,&for_add](const auto& obj){
			auto rsplitted = split(boost::get<ast::record>(&obj.var));
			auto isplitted = split(boost::get<ast::interface>(&obj.var));
			for(auto& s:rsplitted) for_add.emplace_back(std::move(s));
			for(auto& s:isplitted) for_add.emplace_back(std::move(s));

			return !rsplitted.empty() || !isplitted.empty();
	});
	mod.content.erase(remove_begin, mod.content.end());

	for(auto& a:for_add) mod.content.emplace_back(std::move(a));
}

std::vector<ix3::ast::record> split_by_version::split(const ast::record* obj) const
{
	if(!obj) return {};

	helpers::copy_by_version<ast::record, helpers::record_split_traits> cpyer(*obj);
	for(auto& m:obj->members) cpyer.replace_after(m, ast::get<version>(m.meta_params));
	return cpyer.extract_result();
}

std::vector<ix3::ast::interface> split_by_version::split(const ast::interface* obj) const
{
	if(!obj) return {};

	helpers::copy_by_version<ast::interface, helpers::interface_split_traits> cpyer(*obj);
	for(auto& m:obj->mem_funcs) cpyer.replace_after(m, ast::get<version>(m.meta_params));
	for(auto& m:obj->constructors) cpyer.replace_after(m, ast::get<version>(m.meta_params));
	return cpyer.extract_result();
}
