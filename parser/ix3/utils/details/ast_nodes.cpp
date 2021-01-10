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

std::int64_t ix3::utils::details::splash_version(const ast::meta::version& v)
{
	const auto& a = v.major_v;
	const auto& b = v.minor_v;
	return a >= b ? a * a + a + b : a + b * b;
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

boost::json::object type_node::make_json(const compilation_context& ctx) const
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

boost::json::object function_node::make_json(const compilation_context& ctx) const
{
	boost::json::object ret = ast_node::make_json(ctx);
	ret["type"] = "function"sv;
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

boost::json::object fnc_param_node::make_json(const compilation_context& ctx) const
{
	assert(ctx.children(*this).size() == 1);
	boost::json::object ret = ast_node::make_json(ctx);
	ret["type"] = "function_parameter"sv;
	ret["param_t"] = ctx.children(*this)[0]->make_json(ctx);
	ret["req"] = original_node().required;
	return ret;
}

record_node::record_node(ast::record n) : ast_node(std::move(n)) {}

boost::json::object record_node::make_json(const compilation_context& ctx) const
{
	boost::json::object ret = ast_node::make_json(ctx);
	ret["type"] = "record"sv;
	ret["is_exception"] = original_node().use_as_exception;
	boost::json::array& fields=ret["fields"].emplace_array();
	for(auto& f:ctx.children(*this))
		fields.emplace_back(f->make_json(ctx));
	return ret;
}

record_field::record_field(ast::record_item i) : ast_node(std::move(i)) {}

boost::json::object record_field::make_json(const compilation_context& ctx) const
{
	assert(ctx.children(*this).size() == 1);
	boost::json::object ret = ast_node::make_json(ctx);
	ret["type"] = "record_item"sv;
	ret["req"] = original_node().is_required;
	ret["param_t"] = ctx.children(*this)[0]->make_json(ctx);
	return ret;
}

enums::enums(ast::enumeration e) : ast_node(std::move(e)) {}

boost::json::object enums::make_json(const compilation_context& ctx) const
{
	boost::json::object ret = ast_node::make_json(ctx);
	ret["type"]="enum";
	ret["auto_io"] = original_node().gen_io;
	ret["as_flags"] = original_node().use_bitmask;
	boost::json::array& items = ret["items"].emplace_array();
	for(auto& e:original_node().elements) {
		boost::json::object eobj;
		eobj["name"] = e.name;
		eobj["io"] = e.io.empty() ? e.name : e.io;
		items.emplace_back(eobj);
	}
	return ret;
}

interface::interface(ast::interface i) : ast_node(std::move(i)) {}

boost::json::object interface::make_json(const compilation_context& ctx) const
{
	boost::json::object ret = ast_node::make_json(ctx);
	ret["type"] = "interface";
	ret["ex"] = original_node().use_as_exception;
	ret["rinvert"] = original_node().realization_in_client;
	auto& ctors = ret["ctors"].emplace_array();
	auto& funcs = ret["funcs"].emplace_array();
	return ret;
}
