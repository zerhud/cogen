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
		const gen_utils::tree* c, const ix3_compiler* a)
    : container(c)
    , asp(a)
{
	assert(asp);
	assert(container);
}

const ix3_compiler & compilation_context::compiling_aspect() const
{
	assert(asp);
	return *asp;
}

std::pmr::vector<std::pmr::string> compilation_context::naming(std::string_view orig) const
{
	auto first_name = gen_utils::convert(std::string(orig), asp->config().naming);
	return {std::pmr::string(std::move(first_name))};
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

std::optional<gen_utils::variable> ix3_node_base::node_var() const
{
	return std::nullopt;
}

std::pmr::vector<gen_utils::name_t> ix3_node_base::required_links() const
{
	return {};
}
