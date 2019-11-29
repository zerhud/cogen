/*************************************************************************
 * copyright © 2019 hudyaev alexy <hudyaev.alexy@gmail.com>
 * this file is part of modegen.
 * distributed under the gnu affero general public license.
 * see accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "../meta.hpp"
#include "operators/functions.hpp"
#include "copy_by_version_traits.hpp"

using namespace ix3::utils::helpers;
using ix3::ast::meta::version;

std::optional<version> module_split_traits::get_ver(const ast::module& mod)
{
	return mod.version;
}

void module_split_traits::clear_content(ast::module& mod)
{
	mod.content.clear();
}

void module_split_traits::update_version(ast::module& mod, const ast::meta::version& ver)
{
	mod.version = ver;
}

// -------------------------
// record
// -------------------------

std::optional<version> record_split_traits::get_ver(const ast::record& mod)
{
	return ast::get<ast::meta::version>(mod.meta_params);
}

void record_split_traits::clear_content(ast::record& mod)
{
	mod.members.clear();
}

void record_split_traits::update_version(ast::record& mod, const ast::meta::version& ver)
{
	ast::set(mod.meta_params, ver);
}


std::optional<ix3::ast::meta::version> interface_split_traits::get_ver(const ast::interface& mod)
{
	return ast::get<version>(mod.meta_params);
}

void interface_split_traits::clear_content(ast::interface& mod)
{
	mod.mem_funcs.clear();
	mod.constructors.clear();
}

void interface_split_traits::update_version(ast::interface& mod, const ast::meta::version& ver)
{
	ast::set(mod.meta_params, ver);
}

void interface_split_traits::add_child(ast::interface& obj, const ast::constructor& child)
{
	obj.constructors.emplace_back(child);
}

void interface_split_traits::add_child(ast::interface& obj, const ast::function& child)
{
	obj.mem_funcs.emplace_back(child);
}

ix3::ast::function*    interface_split_traits::find_object(ast::interface& obj, const ast::function& child)
{
	for(auto& c:obj.mem_funcs) if(c.name == child.name) return &c;
	return nullptr;
}

ix3::ast::constructor* interface_split_traits::find_object(ast::interface& obj, const ast::constructor& child)
{
	for(auto& c:obj.constructors) if(c.params == child.params) return &c;
	return nullptr;
}

