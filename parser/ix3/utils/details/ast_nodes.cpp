/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "ast_nodes.hpp"

using namespace std::literals;
using ix3::utils::details::function_node;
using ix3::utils::details::fnc_param_node;
using ix3::utils::details::record_node;
using ix3::utils::details::record_field;

std::int64_t ix3::utils::details::splash_version(const ast::meta::version& v)
{
	const auto& a = v.major_v;
	const auto& b = v.minor_v;
	return a >= b ? a * a + a + b : a + b * b;
}

function_node::function_node(ast::function n) : ast_node(std::move(n)) {}

boost::json::object function_node::make_json(const compilation_context& ctx) const
{
	boost::json::object ret = ast_node::make_json(ctx);
	ret["type"] = "function"sv;
	ret["return"] = ast_to_json(original_node().return_type);
	boost::json::array& params=ret["params"].emplace_array();
	for(auto& child:ctx.children(*this))
		params.emplace_back(child->make_json(ctx));
	ctx.compiling_aspect().aspect(*this, ret);
	return ret;
}

fnc_param_node::fnc_param_node(ast::function_parameter n) : ast_node(std::move(n)) {}

boost::json::object fnc_param_node::make_json(const compilation_context& ctx) const
{
	boost::json::object ret = ast_node::make_json(ctx);
	ret["type"] = "function_parameter"sv;
	ret["param_t"] = ast_to_json(original_node().param_type);
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
	boost::json::object ret = ast_node::make_json(ctx);
	ret["type"] = "record_item"sv;
	ret["req"] = original_node().is_required;
	ret["param_t"] = ast_to_json(original_node().param_type);
	return ret;
}
