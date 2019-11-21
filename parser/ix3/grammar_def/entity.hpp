/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>

#include "common.hpp"
#include "grammar/common.hpp"
#include "grammar/entity.hpp"
#include "grammar/functions.hpp"

namespace ix3::text {

auto const record_item_def = meta_set >> is_required >> type >> single_variable_name;
auto const enum_element_def = single_variable_name >> -(lit("=>") >> quoted2_string);
auto const record_def = meta_set
	>> lit("record")
	>> single_variable_name
	>> -(lit("+ex") >> attr(true))
	>> '{' >> *(record_item >> ';') >> '}'
	;

auto const interface_def = meta_set
	>> lit("interface")
	>> single_variable_name
	>> -(lit("+i") >> attr(true))
	>> -(lit("+ex") >> attr(true))
	>> '{'
	>> *(constructor >> ';')
	>> *(function >> ';')
	>> '}'
	;

auto const enumeration_def = meta_set
	>> lit("enum")
	>> single_variable_name
	>> -(lit("+auto_io") >> attr(true)) >> -(lit("+flags") >> attr(true))
	>> '{'
	>> enum_element % ','
	>> '}'
	;

class  record_class : x3::annotate_on_success {} ;
class  interface_class : x3::annotate_on_success {} ;
class  enumeration_class : x3::annotate_on_success {} ;
class  record_item_class : x3::annotate_on_success {} ;
class  enum_element_class : x3::annotate_on_success {} ;

BOOST_SPIRIT_DEFINE( record )
BOOST_SPIRIT_DEFINE( interface )
BOOST_SPIRIT_DEFINE( enumeration )
BOOST_SPIRIT_DEFINE( record_item )
BOOST_SPIRIT_DEFINE( enum_element )

} // namespace ix3::text

