/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ast/file.hpp"
#include "utils/meta.hpp"
#include "ix3_node_base.hpp"

namespace ix3::utils::details {

std::int64_t splash_version(const ast::meta::version& v);

template<typename Ast>
class ast_node : virtual public ix3_node_base {
	Ast ast;
protected:
	boost::json::value ast_to_json(const ix3::ast::variable_name& obj) const {
		boost::json::array ret;
		for (auto& n:obj) ret.emplace_back(n);
		return ret;
	}

public:
	explicit ast_node(Ast n) : ast(std::move(n)) {}

	[[nodiscard]]
	const Ast& original_node() const { return ast; }

	[[nodiscard]] std::string_view name() const override {
		if constexpr (requires{ {ast.name} -> std::convertible_to<std::string_view>; })
		    return ast.name;
		else return "";
	}

	[[nodiscard]]
	std::optional<gen_utils::variable> node_var() const override {
		return std::nullopt;
	}

	[[nodiscard]]
	std::optional<std::uint64_t> version() const override {
		if constexpr (requires{ Ast::meta_params; }) {
			auto ver = ast::get<ast::meta::version>(original_node().meta_params);
			if (ver) return splash_version(*ver);
		}
		return std::nullopt;
	}
};

struct camel_case_as_title : virtual ix3_node_base {
	std::pmr::string cvt_inner_name(gen_utils::name_conversion n) const override ;
};

struct type_node : ast_node<ast::type> {
	type_node(ast::type t);
	std::string_view type_name() const ;
	std::string_view name() const override ;
	boost::json::value make_json(const compilation_context& ctx) const override ;
	std::pmr::vector<gen_utils::name_t> required_links() const override ;
};

struct function_node : ast_node<ast::function> {
	function_node(ast::function n);
	boost::json::value make_json(const compilation_context& ctx) const override ;
	std::string_view inner_name() const override ;
	std::string_view name() const override ;
};

struct ctor_node : ast_node<ast::constructor> {
	ctor_node(ast::constructor n);
	boost::json::value make_json(const compilation_context& ctx) const override ;
	std::string_view inner_name() const override ;
	std::string_view name() const override ;
};

struct fnc_param_node : ast_node<ast::function_parameter> {
	fnc_param_node(ast::function_parameter n);
	boost::json::value make_json(const compilation_context& ctx) const override ;
};

struct record_node : camel_case_as_title, ast_node<ast::record> {
	record_node(ast::record n);
	boost::json::value make_json(const compilation_context& ctx) const override ;
};

struct record_field : ast_node<ast::record_item> {
	record_field(ast::record_item n);
	boost::json::value make_json(const compilation_context& ctx) const override ;
};

struct enums : camel_case_as_title, ast_node<ast::enumeration> {
	enums(ast::enumeration e);
	boost::json::value make_json(const compilation_context& ctx) const override ;
};

struct interface : camel_case_as_title, ast_node<ast::interface> {
	interface(ast::interface i);
	boost::json::value make_json(const compilation_context& ctx) const override ;
	std::string_view link_condition() const override ;
};

} // namespace ix3::utils::details
