/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "selector.hpp"

bool ix3::utils::is_selected(const ast::module_content& cnt, selector sel)
{
	if(cnt.var.type() == typeid(ast::record) && ((sel & selector::record) == selector::record)) return true;
	if(cnt.var.type() == typeid(ast::function) && ((sel & selector::function) == selector::function)) return true;
	if(cnt.var.type() == typeid(ast::interface) && ((sel & selector::interface) == selector::interface)) return true;
	if(cnt.var.type() == typeid(ast::enumeration) && ((sel & selector::enumeration) == selector::enumeration)) return true;
	return false;
}

std::ostream& ix3::utils::operator << (std::ostream& out, selector s)
{
	if(s == selector::record)      return out << "record";
	if(s == selector::function)    return out << "record";
	if(s == selector::interface)   return out << "record";
	if(s == selector::enumeration) return out << "record";
	assert(false);
	return out;
}
