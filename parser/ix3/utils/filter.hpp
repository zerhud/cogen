/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "evaluator.hpp"
#include "selector.hpp"

namespace ix3::utils {

class filter : public evaluator {
public:
	struct request{
		std::string mod_name;
		std::string cnt_name;
		selector sel = selector::all;
	};

	filter(request r);
private:
	void eval_module(ast::module& mod) override ;
	std::vector<ast::module>& extract_result() override ;

	bool check_mod(const ast::module& mod) const ;
	bool check_cnt(const ast::module_content& cnt) const ;

	request query;
	std::vector<ast::module> result;
};

} // namespace ix3::utils
