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
#include "helpers/copy_by_version_traits.hpp"

namespace ix3::utils {

class split_by_version : public evaluator {
public:
	split_by_version(bool dry_run = false);
private:
	typedef helpers::copy_by_version<ast::module, helpers::module_split_traits> cpy_by_ver_t;

	void eval_module(ast::module& mod) override ;
	std::vector<ast::module>& extract_result() override ;

	std::vector<ast::record> split(const ast::record& obj) const ;
	std::vector<ast::interface> split(const ast::interface& obj) const ;

	void on_obj(ast::module& obj) override ;

	bool dry = false;
	std::vector<cpy_by_ver_t> result;
	std::vector<ast::module> finished_result;
};

} // namespace ix3::utils

