/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <regex>
#include "filter.hpp"

ix3::utils::filter::filter(filter::request r ) : query(std::move(r)) {}

void ix3::utils::filter::eval_module(ast::module& mod)
{
	if(!check_mod(mod)) return;

	ast::module rmod = mod;
	rmod.content.clear();

	for(auto& cnt:mod.content) {
		if(is_selected(cnt, query.sel) && check_cnt(cnt))
			rmod.content.emplace_back(std::move(cnt));
	}

	result.emplace_back(std::move(rmod));
}

std::vector<ix3::ast::module>& ix3::utils::filter::extract_result()
{
	return result;
}

bool ix3::utils::filter::check_mod(const ast::module& mod) const
{
	if(query.mod_name.empty()) return true;
	std::regex checker(query.mod_name);
	return std::regex_match(mod.name, checker);
}

bool ix3::utils::filter::check_cnt(const ast::module_content& cnt) const
{
	if(query.cnt_name.empty()) return true;
	std::regex checker(query.cnt_name);
	auto checker_fnc = [&checker](const auto& c){ return std::regex_match(c.name, checker); };
	return boost::apply_visitor(checker_fnc, cnt.var);
}

