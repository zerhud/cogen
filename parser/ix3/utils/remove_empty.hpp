/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "evaluator.hpp"

namespace ix3::utils {

class remove_empty : public evaluator {
	std::vector<ast::module> mods;
	void eval_module(ast::module& mod) override ;
	std::vector<ast::module>& extract_result() override ;
//public:
	//remove_empty() noexcept =default ;
	//~remove_empty() noexcept override =default ;
};

} // namespace ix3::utils
