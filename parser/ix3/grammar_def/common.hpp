/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>

#include "grammar/common.hpp"

namespace ix3::text {

extern decltype(x3::char_)& char_;
extern decltype(x3::space)& space;
using boost::spirit::x3::lit;
using boost::spirit::x3::skip;
using boost::spirit::x3::omit;
using boost::spirit::x3::attr;
using boost::spirit::x3::lexeme;

auto const quoted_string_1_def = *(char_ >> !lit('\'') | lit("\\'") >> x3::attr('\'')) >> char_;
auto const quoted_string_2_def = *(char_ >> !lit('"') | lit("\\\"") >> x3::attr('"')) >> char_;
auto const quoted1_string_def = lexeme[lit("'") >> -quoted_string_1_def >> lit("'")];
auto const quoted2_string_def = lexeme[lit("\"") >> -quoted_string_2_def >> lit("\"")];

auto const single_variable_name_def = lexeme[char_("A-Za-z_") >> *char_("0-9A-Za-z_")];
auto const variable_name_def = single_variable_name % '.';

auto const type_def = variable_name >> -(lit('<') >> type % ',' >> lit('>'));


auto const meta_set_def = *(('@' >> meta_version) | meta_depricated | meta_documentation);
auto const meta_version_def = 'v' >> x3::int_ >> '.' >> x3::int_;
auto const meta_depricated_def = lit("@depricated") >> meta_version >> '(' >> quoted2_string >> ')';
auto const meta_documentation_def = '#' >> lexeme[ *(char_ - x3::eol) >> x3::eol ];

class type_class           : x3::annotate_on_success {};
class variable_name_class  : x3::annotate_on_success {};
class quoted1_string_class : x3::annotate_on_success {};
class quoted2_string_class : x3::annotate_on_success {};
class single_variable_name_class   : x3::annotate_on_success {};

class meta_set : x3::annotate_on_success {} ;
class meta_version : x3::annotate_on_success {} ;
class meta_depricated : x3::annotate_on_success {} ;
class meta_documentation : x3::annotate_on_success {} ;

BOOST_SPIRIT_DEFINE( type )
BOOST_SPIRIT_DEFINE( variable_name )
BOOST_SPIRIT_DEFINE( quoted1_string )
BOOST_SPIRIT_DEFINE( quoted2_string )
BOOST_SPIRIT_DEFINE( single_variable_name  )

BOOST_SPIRIT_DEFINE( meta_set  )
BOOST_SPIRIT_DEFINE( meta_version  )
BOOST_SPIRIT_DEFINE( meta_depricated  )
BOOST_SPIRIT_DEFINE( meta_documentation  )

} // namespace ix3::text
