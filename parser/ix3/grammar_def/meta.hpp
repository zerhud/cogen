/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>

#include "grammar/meta.hpp"
#include "common.hpp"

namespace ix3::text {

auto const meta_set_def = *(('@' >> meta_version) | meta_depricated | meta_documentation | meta_oapi);
auto const meta_oapi_def = lexeme[lit("@oapi") >> omit[+space] >> single_variable_name] >> lit("=") >> (quoted2_string | single_variable_name);
auto const meta_version_def = 'v' >> x3::int_ >> '.' >> x3::int_;
auto const meta_depricated_def = lit("@depricated") >> meta_version >> '(' >> quoted2_string >> ')';
auto const meta_documentation_def = '#' >> lexeme[ *(char_ - x3::eol) >> x3::eol ];

class meta_set : x3::annotate_on_success {} ;
class meta_oapi : x3::annotate_on_success {} ;
class meta_version : x3::annotate_on_success {} ;
class meta_depricated : x3::annotate_on_success {} ;
class meta_documentation : x3::annotate_on_success {} ;

BOOST_SPIRIT_DEFINE( meta_set  )
BOOST_SPIRIT_DEFINE( meta_oapi  )
BOOST_SPIRIT_DEFINE( meta_version  )
BOOST_SPIRIT_DEFINE( meta_depricated  )
BOOST_SPIRIT_DEFINE( meta_documentation  )

} // namespace ix3::text
