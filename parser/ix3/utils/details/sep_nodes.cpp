/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "sep_nodes.hpp"
#include "ast_nodes.hpp"

using namespace std::literals;
using ix3::utils::details::ix3_root_node;
using ix3::utils::details::module_node;
using ix3::utils::details::module_version_node;

std::string_view ix3_root_node::name() const { return "ix3"sv; }

std::optional<std::uint64_t> ix3_root_node::version() const { return 0; }

boost::json::value ix3_root_node::make_json(const compilation_context& ctx) const
{
	boost::json::array cnt;
	for(auto& child:ctx.children(*this))
		cnt.emplace_back(child->make_json(ctx));

	boost::json::object ret;
	ctx.compiling_aspect().aspect(*this, ret);

	return cnt;
}

module_node::module_node(std::string n) : mod_name(std::move(n)) {}
std::string_view module_node::name() const { return mod_name; }
std::optional<std::uint64_t> module_node::version() const { return std::nullopt; }
std::optional<gen_utils::variable> module_node::node_var() const
{
	return gen_utils::variable{"mod", std::pmr::string(mod_name)};
}
boost::json::value module_node::make_json(const compilation_context& ctx) const
{
	boost::json::object ret;
	ret["name"] = boost::json::string_view(name().data(), name().size());
	boost::json::array& content=ret["content"].emplace_array();
	for(auto& child:ctx.children(*this))
		content.emplace_back(child->make_json(ctx));
	ctx.compiling_aspect().aspect(*this, ret);
	return ret;
}


module_version_node::module_version_node(ast::module v) : val(std::move(v))
{
	str_val =
			std::to_string(val.version.major_v) + '.' +
			std::to_string(val.version.minor_v);
}
std::string_view module_version_node::name() const { return str_val; }
std::optional<std::uint64_t> module_version_node::version() const
{
	return splash_version(val.version);
}
std::optional<gen_utils::variable> module_version_node::node_var() const
{
	return gen_utils::variable{"ver", str_val};
}
boost::json::value module_version_node::make_json(const compilation_context& ctx) const
{
	boost::json::object ret;
	ret["type"] = "version";
	ret["value"] = str_val;
	boost::json::array& content=ret["content"].emplace_array();
	for(auto& child:ctx.children(*this))
		content.emplace_back(child->make_json(ctx));
	ctx.compiling_aspect().aspect(*this, ret);
	return ret;
}
