/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <cassert>
#include "part_descriptor.hpp"
#include "exceptions.hpp"

namespace mpg = modegen::pg;
using namespace std::literals;

std::string mpg::to_string(output_lang l)
{
	if(l==output_lang::cpp) return "cpp";
	if(l==output_lang::json) return "json";
	if(l==output_lang::cmake) return "cmake";
	if(l==output_lang::python) return "python";
	if(l==output_lang::javascript) return "javascript";
	assert(false);
	return "";
}

std::string_view mpg::to_string(name_conversion v)
{
	if(v==name_conversion::underscore) return "underscore"sv;
	if(v==name_conversion::camel_case) return "camel_case"sv;
	if(v==name_conversion::title_case) return "title_case"sv;
	if(v==name_conversion::as_is) return "as_is"sv;
	assert(false);
	return ""sv;
}


mpg::from_string::from_string(std::string s)
    : val(std::move(s))
{
}

mpg::from_string::operator mpg::output_lang() const
{
	if(val=="cpp"sv) return output_lang::cpp;
	if(val=="josn"sv) return output_lang::json;
	if(val=="cmake"sv) return output_lang::cmake;
	if(val=="python"sv) return output_lang::python;
	if(val=="javascript"sv) return output_lang::javascript;
	throw errors::error("no such output_lang: "s + std::string(val));
}

mpg::from_string::operator mpg::name_conversion() const
{
	if(val=="underscore"sv) return name_conversion::underscore;
	if(val=="camel_case"sv) return name_conversion::camel_case;
	if(val=="title_case"sv) return name_conversion::title_case;
	if(val=="as_is"sv) return name_conversion::as_is;
	throw errors::error("no such name_conversion: "s + std::string(val));
}

