/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include "common_utils/input/tree.hpp"

namespace ix3::utils::details {

class ix3_node_base;
struct module_node;
struct module_version_node;
struct function_node;
struct record_node;

class ix3_compiler {
public:
	virtual ~ix3_compiler() noexcept =default ;

	virtual const gen_utils::compilation_config& config() const =0 ;

	virtual void aspect(const ix3_node_base& node, boost::json::object& res) const =0 ;
	virtual void aspect(const module_node& node, boost::json::object& res) const =0 ;
	virtual void aspect(const module_version_node& node, boost::json::object& res) const =0 ;
	virtual void aspect(const function_node& node, boost::json::object& res) const =0 ;
	virtual void aspect(const record_node& node, boost::json::object& res) const =0 ;
};

class compilation_context  {
	const gen_utils::tree* container;
	const ix3_compiler* asp;
public:
	explicit compilation_context(const gen_utils::tree* c, const ix3_compiler* a);

	[[nodiscard]] std::pmr::vector<const ix3_node_base*> children(
			const ix3_node_base& par) const ;

	const ix3_compiler& compiling_aspect() const ;

	[[nodiscard]]
	std::pmr::vector<std::pmr::string> naming(std::string_view orig) const ;
};

class ix3_node_base : public gen_utils::data_node {
public:
	[[nodiscard]]
	std::optional<gen_utils::variable> node_var() const override ;
	std::pmr::vector<gen_utils::name_t> required_links() const override ;

	[[nodiscard]]
	virtual boost::json::object make_json(const compilation_context& ctx) const =0 ;
};

} // namespace ix3::utils::details {
