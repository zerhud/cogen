/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "checker.hpp"
#include "meta.hpp"

namespace ix3::utils::details {

template<typename T>
void require_mod_version_is_less(ast::module& mod, T& obj)
{
	auto obj_ver = ast::get<ast::meta::version>(obj.meta_params);
	if(obj_ver && *obj_ver < mod.version)
		throw checker::parent_version_is_greater();
}

template<typename Parent, typename T>
void require_parent_version_is_less(Parent& par, T& obj)
{
	auto obj_ver = ast::get<ast::meta::version>(obj.meta_params);
	auto par_ver = ast::get<ast::meta::version>(par.meta_params);
	if(obj_ver && par_ver && *obj_ver < par_ver)
		throw checker::parent_version_is_greater();
}

} // namespace ix3::utils::details

void ix3::utils::checker::on_obj(ast::module& obj)
{
	auto obj_ver = ast::get<ast::meta::version>(obj.meta_params);
	if(obj_ver && *obj_ver != obj.version)
		throw module_version_mismatch();

	if(!obj_ver) obj.meta_params.cnt.emplace_back(obj.version);

	result.emplace_back(obj);
}

void ix3::utils::checker::on_obj(ast::record& obj)
{
	details::require_mod_version_is_less(module(), obj);
}

void ix3::utils::checker::on_obj(ast::function& obj)
{
	if(parent<ast::module>(0)) {
		details::require_mod_version_is_less(module(), obj);
		if(obj.is_static.has_value()) throw error("free function has static spec");
		if(obj.is_mutable.has_value()) throw error("free function has mutable spec");
		return;
	}

	ast::interface* par = parent<ast::interface>(0);
	assert(par);

	details::require_parent_version_is_less(*par, obj);

	if(!obj.is_mutable.has_value()) throw error("interface function must have mutable spec");
}

void ix3::utils::checker::on_obj(ast::interface& obj)
{
	details::require_mod_version_is_less(module(), obj);
}

void ix3::utils::checker::on_obj(ast::enumeration& obj)
{
	details::require_mod_version_is_less(module(), obj);
}

void ix3::utils::checker::on_obj(ast::record_item& obj)
{
	ast::record* par = parent<ast::record>(0);
	assert(par);

	details::require_parent_version_is_less(*par, obj);
}

void ix3::utils::checker::on_obj(ast::constructor& obj)
{
	ast::interface* par = parent<ast::interface>(0);
	assert(par);

	details::require_parent_version_is_less(*par, obj);
}

void ix3::utils::checker::operator() (ast::file_content file)
{
	for(auto& m:file.modules) trav_module(m, trav_direction::child_first);
}

std::vector<ix3::ast::module> ix3::utils::checker::extract_result()
{
	std::vector<ast::module> ret;
	ret.swap(result);
	return ret;
}

ix3::utils::checker::error::error(std::string e) noexcept : std::runtime_error(e) {}
ix3::utils::checker::parent_version_is_greater::parent_version_is_greater() noexcept
    : error("module version is less then content versin")
{}
ix3::utils::checker::module_version_mismatch::module_version_mismatch() noexcept
    : error("module has a version in meta parameters and it isn't same as in module declaration")
{}
