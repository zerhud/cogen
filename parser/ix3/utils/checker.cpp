/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "checker.hpp"

void ix3::utils::checker::on_obj(ast::module& obj)
{
	result.emplace_back(obj);
}

void ix3::utils::checker::on_obj(ast::record& obj)
{
}

void ix3::utils::checker::on_obj(ast::function& obj)
{
}

void ix3::utils::checker::on_obj(ast::interface& obj)
{
}

void ix3::utils::checker::on_obj(ast::enumeration& obj)
{
}

void ix3::utils::checker::on_obj(ast::record_item& obj)
{
}

void ix3::utils::checker::on_obj(ast::constructor& obj)
{
}

void ix3::utils::checker::operator() (ast::file_content file)
{
	for(auto& m:file.modules) trav_module(m);
}

std::vector<ix3::ast::module> ix3::utils::checker::extract_result()
{
	std::vector<ast::module> ret;
	ret.swap(result);
	return ret;
}

ix3::utils::checker::error::error(std::string e) noexcept : std::runtime_error(e) {}
ix3::utils::checker::mod_version_is_less::mod_version_is_less() noexcept
    : error("module version is less then content versin")
{}
