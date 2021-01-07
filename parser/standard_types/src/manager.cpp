/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "manager.hpp"

using std_types::manager;

std::string_view manager::id() const
{
	return "stdtypes";
}

boost::json::value manager::to_json(
        const gen_utils::compilation_context &cfg,
        const gen_utils::tree &container) const
{
	boost::json::object ret;
	return ret;
}
