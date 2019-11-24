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

bool mi::meta_parameters::operator == (const mi::meta_parameters::documentation& left, const mi::meta_parameters::documentation& right)
{
	return left.body == right.body;
}

bool mi::meta_parameters::operator == (const mi::meta_parameters::deprication& left, const mi::meta_parameters::deprication& right)
{
	return left.message == right.message && left.since == right.since;
}

bool mi::meta_parameters::operator == (const oapi& left, const oapi& right)
{
	return left.value() == right.value();
}

bool mi::operator == (const mi::module& left, const mi::module& right)
{
	return left.name == right.name
		&& left.content.size() == right.content.size()
		&& std::equal(left.content.begin(), left.content.end(), right.content.begin())
		&& left.meta_params == right.meta_params
		;
}

bool mi::operator == (const mi::export_info& left, const mi::export_info& right)
{
	return left.name == right.name && left.type == right.type;
}

bool mi::operator == (const mi::interface& left, const mi::interface& right)
{
	return left.name == right.name
		&& left.mem_funcs.size() == right.mem_funcs.size()
		&& std::equal(left.mem_funcs.begin(), left.mem_funcs.end(), right.mem_funcs.begin())
		&& left.constructors.size() == right.constructors.size()
		&& std::equal(left.constructors.begin(), left.constructors.end(), right.constructors.begin())
		&& left.meta_params == right.meta_params
		&& left.realization_in_client  == right.realization_in_client 
		;
}

bool mi::operator == (const mi::record& left, const mi::record& right)
{
	return left.name == right.name
		&& left.members.size() == right.members.size()
		&& std::equal(left.members.begin(), left.members.end(), right.members.begin())
		&& left.meta_params == right.meta_params
	;
}

bool mi::operator == (const mi::record_item& left, const mi::record_item& right)
{
	return left.name == right.name
		&& left.param_type == right.param_type
		&& left.meta_params == right.meta_params
	;
}

bool mi::operator == (const mi::enumeration& left, const mi::enumeration& right)
{
	return left.name == right.name
		&& left.elements.size() == right.elements.size()
		&& std::equal(left.elements.begin(), left.elements.end(), right.elements.begin())
		&& left.meta_params == right.meta_params
		&& left.gen_io == right.gen_io
		&& left.use_bitmask == right.use_bitmask
	;
}

bool mi::operator == (const mi::enum_element& left, const mi::enum_element& right)
{
	return left.name == right.name
		&& left.io == right.io
	;
}

bool mi::operator == (const mi::constructor_fnc& left, const mi::constructor_fnc& right)
{
	return left.meta_params == right.meta_params
		&& left.func_params.size() == right.func_params.size()
		&& std::equal(left.func_params.begin(), left.func_params.end(), right.func_params.begin())
		;
}

bool mi::operator == (const mi::function& left, const mi::function& right)
{
	return left.name == right.name
		&& left.return_type == right.return_type
		&& left.is_mutable == right.is_mutable
		&& left.is_static == right.is_static
		&& left.func_params.size() == right.func_params.size()
		&& std::equal(left.func_params.begin(), left.func_params.end(), right.func_params.begin())
		&& left.meta_params == right.meta_params
	;
}

bool mi::operator == (const mi::func_param& left, const mi::func_param& right)
{
	return left.name == right.name
		&& left.param_type == right.param_type
		;
}

bool mi::operator == (const mi::type& left, const mi::type& right)
{
	return left.name == right.name
		&& left.sub_types.size() == right.sub_types.size()
		&& std::equal(left.sub_types.begin(), left.sub_types.end(), right.sub_types.begin())
		;
}

bool mi::operator == (const mi::meta_parameters::parameter_set& left, const mi::meta_parameters::parameter_set& right)
{
	return left.set.size() == right.set.size()
		&& std::equal(left.set.begin(), left.set.end(), right.set.begin())
		;
}

bool mi::operator == (const std::vector<mi::module>& left, const std::vector<mi::module>& right)
{
	return left.size() == right.size()
		&& std::equal(left.begin(), left.end(), right.begin())
		;
}

bool mi::operator < (const mi::using_directive& left, const mi::using_directive& right)
{
	return (right.is_system < left.is_system) && (left.mod_name < right.mod_name);
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

mi::module_content mi::copy(const mi::module_content& cnt, mi::copy_method method)
{
	module_content ret;
	auto copier = [&method,&ret](auto& c){ ret = c.copy(method); };
	std::visit(copier, cnt);
	return ret;
}

std::string mi::name(const mi::module_content& cnt)
{
	auto name_extracter = [](const auto& c) { return c.name; };
	return std::visit(name_extracter, cnt);
}

