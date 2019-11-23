/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "traverser.hpp"
#include "ast/file.hpp"

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
public:
	checker() noexcept =default ;

	void operator () (ast::file_content file);
	std::vector<ast::module> extract_result() ;

	struct error : std::runtime_error { error(std::string e) noexcept; };
	struct parent_version_is_greater : error { parent_version_is_greater() noexcept; };
};

} // namespace ix3::utils
