#include "helpers.hpp"


bool modegen::meta_parameters::operator <(const modegen::meta_parameters::version& left, const modegen::meta_parameters::version& right)
{
	return      left.major_v < right.major_v
	        || (left.major_v == right.major_v && left.minor_v < right.minor_v);
}

bool modegen::meta_parameters::operator ==(const modegen::meta_parameters::version& left, const modegen::meta_parameters::version& right)
{
	return left.major_v == right.major_v && left.minor_v == right.minor_v;
}

bool modegen::meta_parameters::operator <=(const modegen::meta_parameters::version& left, const modegen::meta_parameters::version& right)
{
	return left.major_v <= right.major_v && left.minor_v <= right.minor_v;
}

bool modegen::is_selected(const modegen::module_content& cnt, modegen::module_content_selector s)
{
	if(std::holds_alternative<modegen::function>(cnt) && ((s & module_content_selector::function) == module_content_selector::function))
		return true;
	if(std::holds_alternative<modegen::enumeration>(cnt) && ((s & module_content_selector::enumeration) == module_content_selector::enumeration))
		return true;
	if(std::holds_alternative<modegen::record>(cnt) && ((s & module_content_selector::record) == module_content_selector::record))
		return true;
	if(std::holds_alternative<modegen::interface>(cnt) && ((s & module_content_selector::interface) == module_content_selector::interface))
		return true;

	return false;
}

modegen::module_content_selector modegen::which_selected(const modegen::module_content& cnt)
{
	if(std::holds_alternative<modegen::function>(cnt)) return module_content_selector::function;
	if(std::holds_alternative<modegen::enumeration>(cnt)) return module_content_selector::enumeration;
	if(std::holds_alternative<modegen::record>(cnt)) return module_content_selector::record;
	if(std::holds_alternative<modegen::interface>(cnt)) return module_content_selector::interface;

	assert(false);
	return module_content_selector::all;
}

modegen::module_content_selector modegen::from_string(std::string_view v)
{
	module_content_selector ret = module_content_selector::all;
	if(v == "all") ret = module_content_selector::all;
	if(v == "enum") ret = module_content_selector::enumeration;
	if(v == "interface") ret = module_content_selector::interface;
	if(v == "record") ret = module_content_selector::record;
	if(v == "function") ret = module_content_selector::function;
	return ret;
}

bool modegen::operator == (const modegen::using_directive& left, const modegen::using_directive& right)
{
	return left.mod_name == right.mod_name && left.is_from_system() == right.is_from_system();
}

bool modegen::operator == (const modegen::using_directive& left, std::string_view right)
{
	return left.mod_name == right;
}

bool modegen::operator == (const export_info& left, std::string_view right)
{
	return left.name == right;
}
