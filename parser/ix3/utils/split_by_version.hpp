/*************************************************************************
 * copyright © 2019 hudyaev alexy <hudyaev.alexy@gmail.com>
 * this file is part of modegen.
 * distributed under the gnu affero general public license.
 * see accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "meta.hpp"
#include "evaluator.hpp"
#include "ast/file.hpp"
#include "helpers/copy_by_version.hpp"

namespace ix3::utils {

class split_by_version : public evaluator {
public:
	split_by_version(bool dry_run = false);
private:
	typedef helpers::copy_by_version<ast::module, helpers::module_split_traits> cpy_by_ver_t;

	void eval_module(ast::module& mod) override ;
	std::vector<ast::module>& extract_result() override ;

	void on_obj(ast::module& obj) override ;
	void on_obj(ast::record& obj)  override ;
	void on_obj(ast::function& obj) override    { (void)obj; }
	void on_obj(ast::interface& obj) override   { (void)obj; }
	void on_obj(ast::enumeration& obj) override { (void)obj; }
	void on_obj(ast::record_item& obj) override ;
	void on_obj(ast::constructor& obj) override { (void)obj; }

	void on_obj(ast::enum_element& obj) override       { (void)obj; }
	void on_obj(ast::function_parameter& obj) override { (void)obj; }

	void on_obj(ast::meta::set& obj) override           { (void)obj; }
	void on_obj(ast::meta::oapi& obj) override          { (void)obj; }
	void on_obj(ast::meta::version& obj) override       { (void)obj; }
	void on_obj(ast::meta::depricated& obj) override    { (void)obj; }
	void on_obj(ast::meta::documentation& obj) override { (void)obj; }

	bool dry = false;
	std::vector<cpy_by_ver_t> result;
	std::vector<ast::module> finished_result;
};

} // namespace ix3::utils

