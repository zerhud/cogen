/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "common_utils/input/tree.hpp"

namespace ix3::utils::details {

class ix3_node_base : public gen_utils::data_node {
protected:
	[[nodiscard]]
	std::vector<const ix3_node_base*> children(
			const ix3_node_base& par, const gen_utils::tree& con) const;
public:
	std::optional<gen_utils::variable> node_var() const override ;
	
	[[nodiscard]]
	virtual boost::json::object make_json(const gen_utils::tree& con) const =0 ;
};

} // namespace ix3::utils::details {
