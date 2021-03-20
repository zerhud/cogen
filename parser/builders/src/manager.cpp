/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "manager.hpp"
#include "nodes.hpp"

using namespace std::literals;

std::string_view builders::dsl_manager::id() const
{
	return "builders";
}

boost::json::value builders::dsl_manager::to_json(
        const gen_utils::compilation_context& ctx,
        const gen_utils::tree& container) const
{
	auto& root = dynamic_cast<const root_node&>(container.root());
	return root.to_json(container, ctx);
}
