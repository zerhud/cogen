/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
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
using ix3::utils::ix3_manager;
using ix3::utils::to_generic_ast;
using ix3::utils::details::ix3_node_base;

std::string_view ix3_manager::id() const { return "ix3"sv; }
boost::json::value ix3_manager::to_json(
        const gen_utils::compilation_context& ctx,
        const gen_utils::tree& container) const
{
	const ix3_node_base* root;
	if(!ctx.linked_to)
		root = dynamic_cast<const ix3_node_base*>(&container.root());
	else {
		assert(container.node_exists(ctx.linked_to.get()));
		root = dynamic_cast<const ix3_node_base*>(ctx.linked_to.get());
	}
	assert(root != nullptr);
	details::cpp_compiler compiler{&ctx.cfg};
	details::compilation_context inner_ctx( &container, &compiler, &ctx );
	return ctx.linked_to
	        ? root->make_linked_json(inner_ctx)
	        : root->make_json(inner_ctx);
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

	std::shared_ptr<module_major_ver_node> major;
	auto majors = result.children(*parents.back());
	for(auto& m:majors) {
		assert(dynamic_cast<const module_major_ver_node*>(m.get()));
		if(static_cast<const module_major_ver_node*>(m.get())->ver == v.version.major_v) {
			major = std::static_pointer_cast<module_major_ver_node>(m);
			break;
		}
	}
	if(!major) {
		major = std::make_shared<module_major_ver_node>();
		major->ver = v.version.major_v;
		result.add(*parents.back(), major);
	}

	auto node = std::make_shared<module_version_node>(v);
	result.add(*major, node);
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

void to_generic_ast::make_type(ast::type& v, const gen_utils::data_node& par)
{
	auto in = std::make_shared<details::type_node>(v);
	result.add(par, in);
	for(auto& sub:v.sub_types) make_type(sub, *in);
}

void to_generic_ast::on_obj(ast::record& obj)
{
	assert(parents.size()==2);
	auto r = std::make_shared<details::record_node>(obj);
	result.add(*parents.back(), r);
	parents.emplace_back(std::move(r));
}

void to_generic_ast::on_obj(ast::record_item& obj)
{
	auto field = std::make_shared<details::record_field>(obj);
	result.add(*parents.back(), field);
	make_type(obj.param_type, *field);
}

void to_generic_ast::on_obj(ast::function& obj)
{
	auto cur_fnc = std::make_shared<details::function_node>(obj);
	result.add(*parents.back(), cur_fnc);
	parents.emplace_back(cur_fnc);
	make_type(obj.return_type, *cur_fnc);
}

void to_generic_ast::on_obj(ast::function_parameter& obj)
{
	auto param = std::make_shared<details::fnc_param_node>(obj);
	result.add(*parents.back(), param);
	make_type(obj.param_type, *param);
}

void to_generic_ast::on_obj(ast::enumeration& obj)
{
	auto e = std::make_shared<details::enums>(obj);
	result.add(*parents.back(), e);
	parents.emplace_back(std::move(e));
}

void to_generic_ast::on_obj(ast::interface& obj)
{
	auto i = std::make_shared<details::interface>(obj);
	result.add(*parents.back(), i);
	parents.emplace_back(i);
}

void to_generic_ast::on_obj(ast::constructor& obj)
{
	auto c = std::make_shared<details::ctor_node>(obj);
	result.add(*parents.back(), c);
	parents.emplace_back(c);
}

gen_utils::tree to_generic_ast::operator()(std::vector<ast::module> mods)
{
	for(auto& mod:mods) trav_module(mod, trav_direction::paret_first);
	return result;
}

void to_generic_ast::pop_parent()
{
	assert(!parents.empty());
	parents.pop_back();
}
