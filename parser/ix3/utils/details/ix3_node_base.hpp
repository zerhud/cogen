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
#include "ast/meta.hpp"

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
	const gen_utils::compilation_context* gu_ctx;
	std::pmr::vector<gen_utils::import_info> cur_imports;
public:
	explicit compilation_context(
	        const gen_utils::tree* c,
	        const ix3_compiler* a,
	        const gen_utils::compilation_context *gc);

	[[nodiscard]] std::pmr::vector<const ix3_node_base*> children(
			const ix3_node_base& par) const ;
	ix3::ast::meta::version cur_ver(const ix3_node_base& cur) const ;

	const ix3_compiler& compiling_aspect() const ;

	boost::json::object linked_json(const ix3_node_base& node) const ;

	[[nodiscard]]
	std::pmr::vector<std::pmr::string> naming(
	        const ix3_node_base& target,
	        std::string_view orig) const ;
};

class ix3_node_base : public gen_utils::data_node {
public:
	[[nodiscard]]
	std::optional<gen_utils::variable> node_var() const override ;
	std::pmr::vector<gen_utils::name_t> required_links() const override ;
	std::string_view link_condition() const override {return "";}
	std::optional<gen_utils::import_file> imports_modification() const override { return std::nullopt; }
	virtual std::string_view inner_name() const {return name();}

	[[nodiscard]]
	virtual boost::json::value make_json(const compilation_context& ctx) const =0 ;
	[[nodiscard]]
	virtual std::pmr::string cvt_inner_name(gen_utils::name_conversion to) const ;
protected:
	boost::json::object make_inner_json(const compilation_context& ctx) const;
	virtual boost::json::value make_meta_json(const compilation_context& ctx) const ;
};

} // namespace ix3::utils::details {
