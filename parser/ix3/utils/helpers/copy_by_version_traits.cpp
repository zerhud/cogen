/*************************************************************************
 * copyright © 2019 hudyaev alexy <hudyaev.alexy@gmail.com>
 * this file is part of modegen.
 * distributed under the gnu affero general public license.
 * see accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "../meta.hpp"
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

