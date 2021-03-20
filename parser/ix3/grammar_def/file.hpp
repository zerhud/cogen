/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>

#include "common.hpp"
#include "grammar/meta.hpp"
#include "grammar/common.hpp"
#include "grammar/entity.hpp"
#include "grammar/functions.hpp"
#include "grammar/file.hpp"

namespace ix3::text {

auto const include_st_def   = lit("include") >> quoted2_string;
auto const file_content_def = *(include_st) >> *(module);

auto const module_def = meta_set
	>> lit("module")
	>> single_variable_name
	>> meta_version
	>> ':'
	>> *(record | (function >> ';') | interface | enumeration)
	;


class  module_class : x3::annotate_on_success {} ;
class  include_st_class : x3::annotate_on_success {} ;
class  file_content_class : x3::annotate_on_success {} ;

BOOST_SPIRIT_DEFINE( module )
BOOST_SPIRIT_DEFINE( include_st )
BOOST_SPIRIT_DEFINE( file_content )

} // namespace ix3::text


