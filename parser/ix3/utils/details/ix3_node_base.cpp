/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "ix3_node_base.hpp"

using ix3::utils::details::ix3_compiler;
using ix3::utils::details::ix3_node_base;
using ix3::utils::details::compilation_context;


compilation_context::compilation_context(
        const gen_utils::tree* c,
        const ix3_compiler* a,
        const gen_utils::compilation_context* gc)
    : container(c)
    , asp(a)
    , gu_ctx(gc)
{
	assert(asp);
	assert(container);
	if(gc->links) cur_imports = gc->links->required_for(*container);
}

const ix3_compiler & compilation_context::compiling_aspect() const
{
	assert(asp);
	return *asp;
}

boost::json::object compilation_context::linked_json(const ix3_node_base& node) const
{
	boost::json::object ret;
	gen_utils::compilation_context ctx = *gu_ctx;
	for(auto& link:cur_imports) {
		if(link.from.node.get() != &node) continue;
		ctx.linked_to = link.to.node;
		ctx.cfg = link.cfg;
		ret[link.cond] = link.to.owner->to_json(ctx);
	}
	return ret;
}

std::pmr::vector<std::pmr::string> compilation_context::naming(
        const ix3::utils::details::ix3_node_base& target,
        std::string_view orig) const
{
	std::pmr::vector<std::pmr::string> ret;
	for(auto& n:asp->config().naming)
		ret.emplace_back(target.cvt_inner_name(n));
	if(ret.empty())
		ret.emplace_back(target.cvt_inner_name(gen_utils::name_conversion::as_is));
	return ret;
}

std::pmr::vector<const ix3_node_base *> compilation_context::children(
		const ix3_node_base& par) const
{
	assert(container);
	std::pmr::vector<const ix3_node_base *> ret;
	for(auto& c:container->children(par)) {
		assert(dynamic_cast<const ix3_node_base*>(c.get()));
		ret.emplace_back(static_cast<const ix3_node_base*>(c.get()));
	}
	return ret;
}

ix3::ast::meta::version compilation_context::cur_ver(const ix3_node_base& cur) const
{
	if(cur.ast_ver()) return *cur.ast_ver();
	const auto& par = container->parent(cur, [](const auto& n){
				return n.version().has_value();
			});
	auto& ipar = static_cast<const ix3_node_base&>(par);
	assert(ipar.ast_ver().has_value());
	return *ipar.ast_ver();
}

std::optional<gen_utils::variable> ix3_node_base::node_var() const
{
	return std::nullopt;
}

std::pmr::vector<gen_utils::name_t> ix3_node_base::required_links() const
{
	return {};
}

boost::json::object ix3_node_base::make_inner_json(const compilation_context& ctx) const
{
	boost::json::object ret = ctx.linked_json(*this);
	auto meta = make_meta_json(ctx);
	if(!meta.is_null())
		ret["meta"] = meta;
	ret["orig_name"] = inner_name();
	auto list = ctx.naming(*this, inner_name());
	if(list.size()==1) ret["name"] = list[0];
	else {
		auto& nl = ret["name"].emplace_array();
		for(auto& n:list)
			nl.emplace_back(n);
	}
	return ret;
}

boost::json::value ix3_node_base::make_meta_json(const compilation_context& ctx) const
{
	return boost::json::value{};
}

std::optional<ix3::ast::meta::version> ix3_node_base::ast_ver() const
{
	return std::nullopt;
}

std::pmr::string ix3_node_base::cvt_inner_name(gen_utils::name_conversion to) const
{
	return gen_utils::convert(inner_name(), to);
}
