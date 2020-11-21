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

class ix3_node_base;

class compilation_context  {
	const gen_utils::tree* container;
public:
	compilation_context(const gen_utils::tree* c);

	[[nodiscard]] std::pmr::vector<const ix3_node_base*> children(
			const ix3_node_base& par) const ;

	void aspect(const ix3_node_base& node, boost::json::object& res) const ;
};

class ix3_node_base : public gen_utils::data_node {
public:
	std::optional<gen_utils::variable> node_var() const override ;
	
	[[nodiscard]]
	virtual boost::json::object make_json(const compilation_context& ctx) const =0 ;
};

} // namespace ix3::utils::details {
