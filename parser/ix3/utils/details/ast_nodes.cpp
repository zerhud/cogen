/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "ast_nodes.hpp"

using namespace std::literals;
using ix3::utils::details::type_node;
using ix3::utils::details::function_node;
using ix3::utils::details::fnc_param_node;
using ix3::utils::details::record_node;
using ix3::utils::details::record_field;
using ix3::utils::details::enums;
using ix3::utils::details::interface;
using ix3::utils::details::ctor_node;
using ix3::utils::details::camel_case_as_title;

std::int64_t ix3::utils::details::splash_version(const ast::meta::version& v)
{
	return splash_version(v.major_v, v.minor_v);
}

std::int64_t ix3::utils::details::splash_version(std::int64_t a, std::int64_t i)
{
	return a >= i ? a * a + a + i : a + i * i;
}

std::pmr::string camel_case_as_title::cvt_inner_name(
        gen_utils::name_conversion n) const
{
	if(n==gen_utils::name_conversion::camel_case)
		n = gen_utils::name_conversion::title_case;
	return gen_utils::convert(inner_name(), n);
}

type_node::type_node(ast::type t) : ast_node(std::move(t)) { }

std::string_view type_node::type_name() const
{
	return original_node().name.at(0);
}

std::string_view type_node::name() const
{
	return "";
}

boost::json::value type_node::make_json(const compilation_context& ctx) const
{
	boost::json::object ret = ctx.linked_json(*this);
	ret["type"] = "type";
	ret["name"] = ast_to_json(original_node().name);
	boost::json::array& subs = ret["subs"].emplace_array();
	for(auto& child:ctx.children(*this))
		subs.emplace_back(child->make_json(ctx));
	return ret;
}
std::pmr::vector<gen_utils::name_t> type_node::required_links() const
{
	gen_utils::name_t ret;
	for(auto& n:original_node().name)
		ret.emplace_back(n);
	return {ret};
}

function_node::function_node(ast::function n) : ast_node(std::move(n)) {}

boost::json::value function_node::make_json(const compilation_context& ctx) const
{
	boost::json::object ret = ast_node::make_inner_json(ctx);
	ret["type"] = "function";
	auto children = ctx.children(*this);
	assert(1 <= children.size());
	ret["return"] = children[0]->make_json(ctx);
	boost::json::array& params=ret["params"].emplace_array();
	for(auto pos = (++children.begin());pos!=children.end();++pos)
		params.emplace_back((*pos)->make_json(ctx));
	ctx.compiling_aspect().aspect(*this, ret);
	return ret;
}

std::string_view function_node::inner_name() const
{
	return original_node().name;
}

std::string_view function_node::name() const
{
	return "";
}

fnc_param_node::fnc_param_node(ast::function_parameter n) : ast_node(std::move(n)) {}

boost::json::value fnc_param_node::make_json(const compilation_context& ctx) const
{
	assert(ctx.children(*this).size() == 1);
	boost::json::object ret = ast_node::make_inner_json(ctx);
	ret["type"] = "function_parameter";
	ret["param_t"] = ctx.children(*this)[0]->make_json(ctx);
	ret["req"] = original_node().required;
	return ret;
}

record_node::record_node(ast::record n) : ast_node(std::move(n)) {}

boost::json::value record_node::make_json(const compilation_context& ctx) const
{
	boost::json::object ret = ast_node::make_inner_json(ctx);
	ret["type"] = "record";
	ret["is_exception"] = original_node().use_as_exception;
	boost::json::array& fields=ret["fields"].emplace_array();
	for(auto& f:ctx.children(*this))
		fields.emplace_back(f->make_json(ctx));
	return ret;
}

boost::json::value record_node::make_linked_json(const compilation_context& ctx) const
{
	auto ret = make_inner_json(ctx);
	ret["type"] = "record";
	auto mod = ctx.mod_node(*this);
	assert(mod);
	ret["mod"] = mod->make_linked_json(ctx);
	auto ver = ctx.mod_ver(*this);
	assert(ver);
	ret["mod_ver"] = ver->make_linked_json(ctx);
	return ret;
}

record_field::record_field(ast::record_item i) : ast_node(std::move(i)) {}

boost::json::value record_field::make_json(const compilation_context& ctx) const
{
	assert(ctx.children(*this).size() == 1);
	boost::json::object ret = ast_node::make_inner_json(ctx);
	ret["type"] = "record_item";
	ret["req"] = original_node().is_required;
	ret["param_t"] = ctx.children(*this)[0]->make_json(ctx);
	return ret;
}

enums::enums(ast::enumeration e) : ast_node(std::move(e)) {}

boost::json::value enums::make_json(const compilation_context& ctx) const
{
	boost::json::object ret = ast_node::make_inner_json(ctx);
	ret["type"]="enum";
	ret["auto_io"] = original_node().gen_io;
	ret["as_flags"] = original_node().use_bitmask;
	boost::json::array& items = ret["members"].emplace_array();
	for(auto& e:original_node().elements) {
		boost::json::object eobj;
		eobj["name"] = e.name;
		eobj["io"] = e.io.empty() ? e.name : e.io;
		items.emplace_back(eobj);
	}
	return ret;
}

boost::json::value enums::make_linked_json(const compilation_context& ctx) const
{
	boost::json::object ret = ast_node::make_inner_json(ctx);
	ret["type"]="enum";
	auto mod = ctx.mod_node(*this);
	assert(mod);
	ret["mod"] = mod->make_linked_json(ctx);
	auto ver = ctx.mod_ver(*this);
	assert(ver);
	ret["mod_ver"] = ver->make_linked_json(ctx);
	return ret;
}

interface::interface(ast::interface i) : ast_node(std::move(i)) {}
boost::json::value interface::make_json(const compilation_context& ctx) const
{
	boost::json::object ret = ast_node::make_inner_json(ctx);
	ret["type"] = "interface";
	ret["ex"] = original_node().use_as_exception;
	ret["rinvert"] = original_node().realization_in_client;
	boost::json::array ctors, funcs;
	for(auto& c:ctx.children(*this)) {
		if(c->inner_name()=="$ctor"sv)
			ctors.emplace_back(c->make_json(ctx));
		else
			funcs.emplace_back(c->make_json(ctx));
	}
	ret["ctors"] = std::move(ctors);
	ret["funcs"] = std::move(funcs);
	return ret;
}
boost::json::value interface::make_linked_json(const compilation_context& ctx) const
{
	auto ret = make_inner_json(ctx);
	ret["type"] = "interface";
	auto mod = ctx.mod_node(*this);
	assert(mod);
	ret["mod"] = mod->make_linked_json(ctx);
	auto ver = ctx.mod_ver(*this);
	assert(ver);
	ret["mod_ver"] = ver->make_linked_json(ctx);
	return ret;
}
std::string_view interface::link_condition() const
{
	return "ix3";
}


ctor_node::ctor_node(ast::constructor n) : ast_node(std::move(n)) {}
boost::json::value ctor_node::make_json(const compilation_context& ctx) const
{
	boost::json::object ret;
	ret["type"] = "ctor";
	boost::json::array& params=ret["params"].emplace_array();
	for(auto& p:ctx.children(*this))
		params.emplace_back(p->make_json(ctx));
	return ret;
}
std::string_view ctor_node::inner_name() const
{
	return "$ctor";
}
std::string_view ctor_node::name() const
{
	return "";
}
