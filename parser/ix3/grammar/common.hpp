/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once


#include <boost/spirit/home/x3.hpp>
#include "fusion/common.hpp"

namespace ix3::text {

namespace x3 = boost::spirit::x3;

class quoted1_string_class;
class quoted2_string_class;
class single_variable_name_class;
class variable_name_class;
class type_class;

class is_required_class;

const x3::rule<quoted1_string_class, ast::quoted1_string> quoted1_string = "quoted1_string";
const x3::rule<quoted2_string_class, ast::quoted2_string> quoted2_string = "quoted2_string";
const x3::rule<single_variable_name_class, ast::single_variable_name> single_variable_name = "single_variable_name";
const x3::rule<variable_name_class, ast::variable_name> variable_name = "variable_name";
const x3::rule<type_class, ast::type> type = "type";

const x3::rule<is_required_class, bool> is_required = "is_required";

BOOST_SPIRIT_DECLARE(decltype(quoted1_string))
BOOST_SPIRIT_DECLARE(decltype(quoted2_string))
BOOST_SPIRIT_DECLARE(decltype(single_variable_name))
BOOST_SPIRIT_DECLARE(decltype(variable_name))
BOOST_SPIRIT_DECLARE(decltype(type))

BOOST_SPIRIT_DECLARE(decltype(is_required))

} // namespace ix3::text
