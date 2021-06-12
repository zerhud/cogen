/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "traverser.hpp"
#include "ix3/parser.hpp"
#include "ix3/ast/file.hpp"
#include "utils/tree.hpp"
#include "utils/generic_dsl.hpp"


namespace ix3::utils {

class ix3_manager : public gen_utils::dsl_manager {
public:
	std::string_view id() const override ;
	boost::json::value to_json(
	        const gen_utils::compilation_context& ctx,
	        const gen_utils::tree& container) const override ;
};

class to_generic_ast : protected traverser {
	enum class par_entity {module, entity, function};

	std::shared_ptr<ix3_manager> ix3_owner;
	gen_utils::tree result;
	std::vector<gen_utils::node_ptr> parents;

	void new_mod(ast::module& obj) ;
	void new_mod_ver(ast::module& v);

	void make_type(ast::type& v, const gen_utils::data_node& par);

	void on_obj(ast::module& obj) override ;
	void on_obj(ast::record& obj) override;
	void on_obj(ast::record_item& obj) override;
	void on_obj(ast::function& obj) override ;
	void on_obj(ast::function_parameter &obj) override;
	void on_obj(ast::enumeration& obj) override ;
	void on_obj(ast::interface& obj) override ;
	void on_obj(ast::constructor& obj) override ;

	void pop_parent() override ;
public:
	to_generic_ast();
	gen_utils::tree operator()(std::vector<ast::module> mods);
};

class ix3_loader :
        public parser
      , public gen_utils::generic_sdl
{
	bool was_add = false;
public:
	ix3_loader(include_solver is) : parser(std::move(is)) {}
	bool empty() const override { return !was_add; }
	std::string_view name() const override { return "ix3"; }
	void finish_loads() override { parser::finish_loads(); }
	gen_utils::tree data() const override { return to_generic_ast()(result()); }
	void add(std::filesystem::path file) override {
		was_add = true;
		parse(std::move(file));
	}
};

} // namespace ix3::utils
