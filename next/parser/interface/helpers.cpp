#include "helpers.hpp"


bool modegen::parser::interface::meta_parameters::operator <(const modegen::parser::interface::meta_parameters::version& left, const modegen::parser::interface::meta_parameters::version& right)
{
	return      left.major_v < right.major_v
	        || (left.major_v == right.major_v && left.minor_v < right.minor_v);
}

bool modegen::parser::interface::meta_parameters::operator ==(const modegen::parser::interface::meta_parameters::version& left, const modegen::parser::interface::meta_parameters::version& right)
{
	return left.major_v == right.major_v && left.minor_v == right.minor_v;
}

bool modegen::parser::interface::meta_parameters::operator <=(const modegen::parser::interface::meta_parameters::version& left, const modegen::parser::interface::meta_parameters::version& right)
{
	return left.major_v <= right.major_v && left.minor_v <= right.minor_v;
}

bool modegen::parser::interface::is_selected(const modegen::parser::interface::module_content& cnt, modegen::parser::interface::module_content_selector s)
{
	if(std::holds_alternative<modegen::parser::interface::function>(cnt) && ((s & module_content_selector::function) == module_content_selector::function))
		return true;
	if(std::holds_alternative<modegen::parser::interface::enumeration>(cnt) && ((s & module_content_selector::enumeration) == module_content_selector::enumeration))
		return true;
	if(std::holds_alternative<modegen::parser::interface::record>(cnt) && ((s & module_content_selector::record) == module_content_selector::record))
		return true;
	if(std::holds_alternative<modegen::parser::interface::interface>(cnt) && ((s & module_content_selector::interface) == module_content_selector::interface))
		return true;

	return false;
}

modegen::parser::interface::module_content_selector modegen::parser::interface::which_selected(const modegen::parser::interface::module_content& cnt)
{
	if(std::holds_alternative<modegen::parser::interface::function>(cnt)) return module_content_selector::function;
	if(std::holds_alternative<modegen::parser::interface::enumeration>(cnt)) return module_content_selector::enumeration;
	if(std::holds_alternative<modegen::parser::interface::record>(cnt)) return module_content_selector::record;
	if(std::holds_alternative<modegen::parser::interface::interface>(cnt)) return module_content_selector::interface;

	assert(false);
	return module_content_selector::all;
}

modegen::parser::interface::module_content_selector modegen::parser::interface::from_string(std::string_view v)
{
	module_content_selector ret = module_content_selector::all;
	if(v == "all") ret = module_content_selector::all;
	if(v == "enum") ret = module_content_selector::enumeration;
	if(v == "interface") ret = module_content_selector::interface;
	if(v == "record") ret = module_content_selector::record;
	if(v == "function") ret = module_content_selector::function;
	return ret;
}

bool modegen::parser::interface::operator == (const modegen::parser::interface::using_directive& left, const modegen::parser::interface::using_directive& right)
{
	return left.mod_name == right.mod_name && left.is_from_system() == right.is_from_system();
}

bool modegen::parser::interface::operator == (const modegen::parser::interface::using_directive& left, std::string_view right)
{
	return left.mod_name == right;
}

bool modegen::parser::interface::operator == (const export_info& left, std::string_view right)
{
	return left.name == right;
}

