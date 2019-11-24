/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "evaluator.hpp"

std::vector<ix3::ast::module>& ix3::utils::evaluator::operator() (std::vector<ast::module>& mods)
{
	for(auto& m:mods) eval_module(m);
	extract_result().swap(mods);
	return mods;
}
