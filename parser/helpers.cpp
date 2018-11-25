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
