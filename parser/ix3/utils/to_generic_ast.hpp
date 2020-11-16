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

class to_generic_ast : protected traverser {
	enum class par_entity {module, entity, function};

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
