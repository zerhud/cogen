/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>

#include "meta.hpp"
#include "common.hpp"
#include "grammar/meta.hpp"
#include "grammar/functions.hpp"

namespace ix3::text {

auto const is_static_def = lit("static") >> attr(true);
auto const is_mutable_def = (lit("mutable") >> attr(true)) | (lit("const") >> attr(false));

auto const function_parameter_def = is_required >> type >> single_variable_name;
auto const constructor_def = meta_set >> lit("constructor") >> omit['('] >> -(function_parameter % ',') >> omit[')'];
auto const function_def =
	   meta_set
	>> -is_static
	>> type
	>> single_variable_name
	>> '(' >> -(function_parameter % ',') >> ')'
	>> -is_mutable
	;


class  is_static_clas : x3::annotate_on_success {} ;
class  is_mutable_clas : x3::annotate_on_success {} ;

class  function_calss : x3::annotate_on_success {} ;
class  constructor_calss : x3::annotate_on_success {} ;
class  function_parameter_calss : x3::annotate_on_success {} ;

BOOST_SPIRIT_DEFINE( is_static )
BOOST_SPIRIT_DEFINE( is_mutable )

BOOST_SPIRIT_DEFINE( function )
BOOST_SPIRIT_DEFINE( constructor )
BOOST_SPIRIT_DEFINE( function_parameter )

} // namespace ix3::text

