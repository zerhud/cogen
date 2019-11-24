/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "traverser.hpp"

namespace ix3::utils {

class evaluator : public traverser {
public:
	std::vector<ast::module>& operator () (std::vector<ast::module>& mods);
private:
	virtual void eval_module(ast::module& mod) =0 ;
	virtual std::vector<ast::module>& extract_result() =0 ;
};

} // namespace ix3::utils
