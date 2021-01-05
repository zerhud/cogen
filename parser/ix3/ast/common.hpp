/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>

namespace ix3::ast {

namespace x3 = boost::spirit::x3;

using string_t = std::string;

template<typename T>
using forward_ast = x3::forward_ast<T>;

template<typename... Args>
using variant_t = x3::variant<Args...>;

struct quoted1_string : string_t, x3::position_tagged {};
struct quoted2_string : string_t, x3::position_tagged {};
struct single_variable_name : string_t, x3::position_tagged {};
struct variable_name : std::vector<string_t>, x3::position_tagged {};

struct type : x3::position_tagged {
	variable_name name;
	std::vector<forward_ast<type>> sub_types;
};

} // namespace ix3::ast
