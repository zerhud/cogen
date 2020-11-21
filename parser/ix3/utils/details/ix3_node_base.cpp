/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "ix3_node_base.hpp"

using ix3::utils::details::ix3_node_base;

std::optional<gen_utils::variable> ix3_node_base::node_var() const
{
	return std::nullopt;
}

std::vector<const ix3_node_base *> ix3_node_base::children(
		const ix3_node_base& par, const gen_utils::tree& con) const
{
	std::vector<const ix3_node_base*> ret;
	for(auto& c:con.children(par)) {
		assert(dynamic_cast<const ix3_node_base*>(c.get()));
		ret.emplace_back(static_cast<const ix3_node_base*>(c.get()));
	}
	return ret;
}
