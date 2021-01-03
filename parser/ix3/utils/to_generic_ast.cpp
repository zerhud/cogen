/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "to_generic_ast.hpp"
#include "meta.hpp"

#include "details/ix3_node_base.hpp"
#include "details/ast_nodes.hpp"
#include "details/sep_nodes.hpp"
#include "details/cpp_compiler.hpp"

using namespace std::literals;
using ix3::utils::to_generic_ast;
using ix3::utils::ix3_manager;

std::string_view ix3_manager::id() const { return "ix3"sv; }
boost::json::value ix3_manager::to_json(
        const gen_utils::compilation_context& ctx,
        const gen_utils::tree& container) const
{
	using details::ix3_root_node;
	assert(dynamic_cast<const ix3_root_node*>(&container.root()) != nullptr);
	const auto* root = static_cast<const ix3_root_node*>(&container.root());
	assert(ctx.cfg.name==gen_utils::compiler::cpp);
	details::cpp_compiler compiler{&ctx.cfg};
	details::compilation_context inner_ctx( &container, &compiler, &ctx );
	return root->make_json(inner_ctx);
}

to_generic_ast::to_generic_ast()
    : ix3_owner(std::make_shared<ix3_manager>())
    , result(std::make_shared<details::ix3_root_node>(), ix3_owner)
{
}

void to_generic_ast::new_mod(ast::module& obj)
{
	using namespace details;
	auto mod_node = std::make_shared<module_node>(obj.name);
	result.add(result.root(), mod_node);
	parents.emplace_back(mod_node);
	new_mod_ver(obj);
}

void to_generic_ast::new_mod_ver(ast::module& v)
{
	using namespace details;
	auto node = std::make_shared<module_version_node>(v);
	result.add(*parents.back(), node);
	parents.emplace_back(node);
}

void to_generic_ast::on_obj(ast::module &obj)
{
	parents.clear();
	auto mods = result.children(result.root());
	for(auto& m:mods)
		if(m->name()==obj.name) {
			parents.emplace_back(m);
			new_mod_ver(obj);
			return;
		}
	new_mod(obj);
}

void to_generic_ast::on_obj(ast::record& obj)
{
	assert(parents.size()<=2);
	if(parents.size()!=2) parents.resize(2);
	auto par = parents.back();
	parents.emplace_back(std::make_shared<details::record_node>(obj));
	result.add(*par, parents.back());
}

void to_generic_ast::on_obj(ast::record_item& obj)
{
	result.add(*parents.back(), std::make_shared<details::record_field>(obj));
}

void to_generic_ast::on_obj(ast::function& obj)
{
	cur_fnc = std::make_shared<details::function_node>(obj);
	result.add(*parents.back(), cur_fnc);
}

void to_generic_ast::on_obj(ast::function_parameter& obj)
{
	assert(cur_fnc);
	result.add(*cur_fnc, std::make_shared<details::fnc_param_node>(obj));
}

gen_utils::tree to_generic_ast::operator()(std::vector<ast::module> mods)
{
	for(auto& mod:mods) trav_module(mod, trav_direction::paret_first);
	return result;
}
