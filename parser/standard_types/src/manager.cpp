/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "manager.hpp"
#include "nodes.hpp"

using std_types::manager;

std::string_view manager::id() const
{
	return "stdtypes";
}

boost::json::value manager::to_json(
        const gen_utils::compilation_context& cfg,
        const gen_utils::tree& container) const
{
	if(cfg.linked_to && !container.node_exists(cfg.linked_to.get()))
		throw std::runtime_error("std_type::to_json node doen't contains in the container");
	const base_node* linked = static_cast<const base_node*>(cfg.linked_to.get());
	if(!linked) linked = static_cast<const base_node*>(&container.root());
	return linked->to_json(container);
}
