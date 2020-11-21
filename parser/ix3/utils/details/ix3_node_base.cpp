/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "ix3_node_base.hpp"

using ix3::utils::details::ix3_node_base;
using ix3::utils::details::compilation_context;

compilation_context::compilation_context(const gen_utils::tree* c)
    : container(c)
{
	assert(container);
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

void compilation_context::aspect(const ix3_node_base& node, boost::json::object& res) const
{
	(void)node;
	(void)res;
}

std::optional<gen_utils::variable> ix3_node_base::node_var() const
{
	return std::nullopt;
}
