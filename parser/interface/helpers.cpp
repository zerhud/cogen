/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "helpers.hpp"

namespace mi = modegen::parser::interface;


bool mi::meta_parameters::operator <(const mi::meta_parameters::version& left, const mi::meta_parameters::version& right)
{
	return      left.major_v < right.major_v
	        || (left.major_v == right.major_v && left.minor_v < right.minor_v);
}

bool mi::meta_parameters::operator ==(const mi::meta_parameters::version& left, const mi::meta_parameters::version& right)
{
	return left.major_v == right.major_v && left.minor_v == right.minor_v;
}

bool mi::meta_parameters::operator <=(const mi::meta_parameters::version& left, const mi::meta_parameters::version& right)
{
	return left.major_v <= right.major_v && left.minor_v <= right.minor_v;
}

bool mi::is_selected(const mi::module_content& cnt, mi::module_content_selector s)
{
	if(std::holds_alternative<mi::function>(cnt) && ((s & module_content_selector::function) == module_content_selector::function))
		return true;
	if(std::holds_alternative<mi::enumeration>(cnt) && ((s & module_content_selector::enumeration) == module_content_selector::enumeration))
		return true;
	if(std::holds_alternative<mi::record>(cnt) && ((s & module_content_selector::record) == module_content_selector::record))
		return true;
	if(std::holds_alternative<mi::interface>(cnt) && ((s & module_content_selector::interface) == module_content_selector::interface))
		return true;

	return false;
}

mi::module_content_selector mi::which_selected(const mi::module_content& cnt)
{
	if(std::holds_alternative<mi::function>(cnt)) return module_content_selector::function;
	if(std::holds_alternative<mi::enumeration>(cnt)) return module_content_selector::enumeration;
	if(std::holds_alternative<mi::record>(cnt)) return module_content_selector::record;
	if(std::holds_alternative<mi::interface>(cnt)) return module_content_selector::interface;

	assert(false);
	return module_content_selector::all;
}

mi::module_content_selector mi::from_string(std::string_view v)
{
	module_content_selector ret = module_content_selector::all;
	if(v == "all") ret = module_content_selector::all;
	if(v == "enum") ret = module_content_selector::enumeration;
	if(v == "interface") ret = module_content_selector::interface;
	if(v == "record") ret = module_content_selector::record;
	if(v == "function") ret = module_content_selector::function;
	return ret;
}

bool mi::operator == (const mi::using_directive& left, const mi::using_directive& right)
{
	return left.mod_name == right.mod_name && left.is_from_system() == right.is_from_system();
}

bool mi::operator == (const mi::using_directive& left, std::string_view right)
{
	return left.mod_name == right;
}

bool mi::operator == (const export_info& left, std::string_view right)
{
	return left.name == right;
}

mi::module mi::copy(const mi::module& mod, mi::copy_method method)
{
	mi::module ret;

	if(method==copy_method::deep) ret = mod;
	else {
		// uncomment the next line to get the sizeof at compile time
		// char (*__printer)[sizeof( module )] = 1;
		static_assert(sizeof(mod)==160, "field set in module was changed?");

		ret.name = mod.name;
		ret.meta_params = mod.meta_params;
		ret.imports = mod.imports;
		ret.exports = mod.exports;
		ret.file_name = mod.file_name;
	}

	return ret;
}

