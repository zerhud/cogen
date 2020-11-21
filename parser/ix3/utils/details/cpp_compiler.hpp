/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ix3_node_base.hpp"

namespace ix3::utils::details {

struct cpp_compiler : ix3_compiler {
	const gen_utils::compilation_config* config;

	explicit cpp_compiler(const gen_utils::compilation_config* c);

	void aspect(const ix3_node_base& node, boost::json::object& res) const override;
	void aspect(const module_node& node, boost::json::object& res) const override ;
	void aspect(const module_version_node& node, boost::json::object& res) const override;
	void aspect(const function_node& node, boost::json::object& res) const override ;
	void aspect(const record_node& node, boost::json::object& res) const override ;
};

} // namespace ix3::utils::details
