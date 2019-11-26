/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "remove_empty.hpp"

using namespace ix3::utils;

void remove_empty::eval_module(ast::module& mod)
{
	if(!mod.content.empty())
		mods.emplace_back(std::move(mod));
}

std::vector<ix3::ast::module>& remove_empty::extract_result()
{
	return mods;
}
