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
