/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "naming.hpp"

using namespace ix3::utils;
namespace ast = ix3::ast;

naming::naming(gen_utils::name_conversion nc) : name_to(nc) {}

void naming::eval_module(ast::module& mod)
{
	trav_module(mod);
	mods.emplace_back(std::move(module()));
}

std::vector<ix3::ast::module>& naming::extract_result()
{
	return mods;
}

void naming::on_obj(ast::module& obj)
{
	obj.name = gen_utils::convert(obj.name, name_to);
}

void  naming::on_obj(ast::record& obj)
{
	obj.name.assign(gen_utils::convert(obj.name, name_to));
}

void  naming::on_obj(ast::function& obj)
{
	obj.name.assign(gen_utils::convert(obj.name, name_to));
}

void  naming::on_obj(ast::interface& obj)
{
	obj.name.assign(gen_utils::convert(obj.name, name_to));
}

void  naming::on_obj(ast::enumeration& obj)
{
	obj.name.assign(gen_utils::convert(obj.name, name_to));
}

void  naming::on_obj(ast::record_item& obj)
{
	obj.name.assign(gen_utils::convert(obj.name, name_to));
}

void  naming::on_obj(ast::constructor& obj)
{
	(void) obj;
}

void  naming::on_obj(ast::enum_element& obj)
{
	obj.name.assign(gen_utils::convert(obj.name, name_to));
}

void  naming::on_obj(ast::function_parameter& obj)
{
	obj.name.assign(gen_utils::convert(obj.name, name_to));
}

void  naming::on_obj(ast::meta::set& obj)
{
	(void) obj;
}

void  naming::on_obj(ast::meta::oapi& obj)
{
	(void) obj;
}

void  naming::on_obj(ast::meta::version& obj)
{
	(void) obj;
}

void  naming::on_obj(ast::meta::depricated& obj)
{
	(void) obj;
}

void  naming::on_obj(ast::meta::documentation& obj)
{
	(void) obj;
}

