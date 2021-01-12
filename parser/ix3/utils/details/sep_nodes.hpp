/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ast/file.hpp"
#include "ix3_node_base.hpp"

namespace ix3::utils::details {

struct ix3_root_node : ix3_node_base {
	std::string_view name() const override ;
	std::optional<std::uint64_t> version() const override ;
	boost::json::value make_json(const compilation_context& ctx) const override ;
};

struct module_node : ix3_node_base {
	std::string mod_name;

	module_node(std::string n);

	std::string_view name() const override ;

	std::optional<std::uint64_t> version() const override ;

	std::optional<gen_utils::variable> node_var() const override ;

	boost::json::value make_json(const compilation_context& ctx) const override;
};

struct module_version_node : ix3_node_base {
	ast::module val;
	std::pmr::string str_val;
	module_version_node(ast::module v) ;

	std::string_view name() const override ;
	std::optional<std::uint64_t> version() const override ;
	std::optional<gen_utils::variable> node_var() const override ;
	boost::json::value make_json(const compilation_context& ctx) const override;
};


} // namespace ix3::utils::details
