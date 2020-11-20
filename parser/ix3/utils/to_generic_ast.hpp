/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "traverser.hpp"
#include "ix3/ast/file.hpp"
#include "common_utils/input/tree.hpp"

namespace ix3::utils {

namespace details {

class ix3_node_base : public gen_utils::data_node {
protected:
	[[nodiscard]]
	std::vector<const ix3_node_base*> children(
			const ix3_node_base& par, const gen_utils::tree& con) const;
public:
	[[nodiscard]]
	virtual boost::json::object make_json(const gen_utils::tree& con) const =0 ;
};

} // namespace details

class ix3_manager : public gen_utils::dsl_manager {
public:
	std::string_view id() const override ;
	boost::json::value to_json(const gen_utils::tree& container) const override ;
};

class to_generic_ast : protected traverser {
	enum class par_entity {module, entity, function};

	std::shared_ptr<ix3_manager> ix3_owner;
	gen_utils::tree result;
	std::vector<gen_utils::node_ptr> parents;
	gen_utils::node_ptr cur_fnc;

	void new_mod(ast::module& obj) ;
	void new_mod_ver(ast::module& v);

	void on_obj(ast::module& obj) override ;
	void on_obj(ast::record& obj) override;
	void on_obj(ast::record_item& obj) override;
	void on_obj(ast::function& obj) override ;
	void on_obj(ast::function_parameter &obj) override;
public:
	to_generic_ast();
	gen_utils::tree operator()(std::vector<ast::module> mods);
};

} // namespace ix3::utils
