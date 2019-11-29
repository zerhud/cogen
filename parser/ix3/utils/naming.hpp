/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "evaluator.hpp"
#include "common_utils/naming.hpp"

namespace ix3::utils {

class naming : public evaluator {
	gen_utils::name_conversion name_to;
	std::vector<ast::module> mods;

	void eval_module(ast::module& mod) override ;
	std::vector<ast::module>& extract_result() override ;

	void on_obj(ast::module& obj) override ;
	void on_obj(ast::record& obj) override ;
	void on_obj(ast::function& obj) override ;
	void on_obj(ast::interface& obj) override ;
	void on_obj(ast::enumeration& obj) override ;
	void on_obj(ast::record_item& obj) override ;
	void on_obj(ast::constructor& obj) override ;

	void on_obj(ast::enum_element& obj) override ;
	void on_obj(ast::function_parameter& obj) override ;

	void on_obj(ast::meta::set& obj) override ;
	void on_obj(ast::meta::oapi& obj) override ;
	void on_obj(ast::meta::version& obj) override ;
	void on_obj(ast::meta::depricated& obj) override ;
	void on_obj(ast::meta::documentation& obj) override ;
public:
	naming(gen_utils::name_conversion nc);
};

} // namespace ix3::utils

