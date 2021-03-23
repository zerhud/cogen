/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>

#include "env.hpp"
#include "grammar/common.hpp"

namespace ix3::text {

struct error_handler
{
	template <typename Iterator, typename Exception, typename Context>
	x3::error_handler_result on_error(
	        Iterator& first, Iterator const& last
	      , Exception const& x, Context const& context)
	{
		auto& error_handler = x3::get<x3::error_handler_tag>(context).get();
		auto& env = x3::get<parser_env_tag>(context).get();
		error_handler(x.where(), env.msg(x.which()));
		env.on_error();
		return x3::error_handler_result::fail;
	}
};


extern decltype(x3::char_)& char_;
extern decltype(x3::space)& space;
using boost::spirit::x3::lit;
using boost::spirit::x3::skip;
using boost::spirit::x3::omit;
using boost::spirit::x3::attr;
using boost::spirit::x3::lexeme;

auto const quoted_string_1_def = *(char_ >> !lit('\'') | lit("\\'") >> x3::attr('\'')) >> char_;
auto const quoted_string_2_def = *(char_ >> !lit('"') | lit("\\\"") >> x3::attr('"')) >> char_;
auto const quoted1_string_def = lexeme[lit("'") >> -quoted_string_1_def > lit("'")];
auto const quoted2_string_def = lexeme[lit("\"") >> -quoted_string_2_def > lit("\"")];

auto const single_variable_name_def = lexeme[char_("A-Za-z_") >> *char_("0-9A-Za-z_")];
auto const variable_name_def = single_variable_name % '.';

auto const type_def = variable_name >> -(lit('<') >> type % ',' > lit('>'));

auto const is_required_def = omit['-'] >> attr(false) | omit['+'] >> attr(true);

struct type_class           : error_handler, x3::annotate_on_success {};
struct variable_name_class  : error_handler, x3::annotate_on_success {};
struct quoted1_string_class : error_handler, x3::annotate_on_success {};
struct quoted2_string_class : error_handler, x3::annotate_on_success {};
struct single_variable_name_class   : error_handler, x3::annotate_on_success {};

struct is_required_class : x3::annotate_on_success {} ;

BOOST_SPIRIT_DEFINE( type )
BOOST_SPIRIT_DEFINE( variable_name )
BOOST_SPIRIT_DEFINE( quoted1_string )
BOOST_SPIRIT_DEFINE( quoted2_string )
BOOST_SPIRIT_DEFINE( single_variable_name  )

BOOST_SPIRIT_DEFINE( is_required  )

} // namespace ix3::text
