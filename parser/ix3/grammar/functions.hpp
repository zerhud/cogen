/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once


#include <boost/spirit/home/x3.hpp>
#include "fusion/functions.hpp"

namespace ix3::text {

namespace x3 = boost::spirit::x3;

struct function_class;
struct constructor_class;
struct function_parameter_class;

struct is_static_class;
struct is_mutable_class;

const x3::rule<is_static_class, bool> is_static = "is_static";
const x3::rule<is_mutable_class, bool> is_mutable = "is_mutable";

const x3::rule<function_class, ast::function> function = "function";
const x3::rule<constructor_class, ast::constructor> constructor = "constructor";
const x3::rule<function_parameter_class, ast::function_parameter> function_parameter = "function_parameter";

BOOST_SPIRIT_DECLARE(decltype(function))
BOOST_SPIRIT_DECLARE(decltype(constructor))
BOOST_SPIRIT_DECLARE(decltype(function_parameter))

BOOST_SPIRIT_DECLARE(decltype(is_static))
BOOST_SPIRIT_DECLARE(decltype(is_mutable))

} // namespace ix3::text

