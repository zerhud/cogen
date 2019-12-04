/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "traverser.hpp"
#include "ix3/ast/file.hpp"

namespace ix3::utils {

class checker : public traverser {
	std::vector<ast::module> result;

	void on_obj(ast::module& obj)      override ;
	void on_obj(ast::record& obj)      override ;
	void on_obj(ast::function& obj)    override ;
	void on_obj(ast::interface& obj)   override ;
	void on_obj(ast::enumeration& obj) override ;
	void on_obj(ast::record_item& obj) override ;
	void on_obj(ast::constructor& obj) override ;

	ast::module* find_same(const ast::module& mod) ;
	void merge(ast::module& to, ast::module& from);
public:
	checker() noexcept =default ;

	void operator () (ast::file_content file);
	std::vector<ast::module> extract_result() ;

	struct error : std::runtime_error { error(std::string e) noexcept; };
	struct parent_version_is_greater : error { parent_version_is_greater() noexcept; };
	struct module_version_mismatch : error { module_version_mismatch() noexcept ; };
};

} // namespace ix3::utils
