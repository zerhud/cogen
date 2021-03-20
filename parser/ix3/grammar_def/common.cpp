/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "common.hpp"
#include "../parse.hpp"

namespace ix3::text {

decltype(x3::char_)& char_ = x3::char_;
decltype(x3::space)& space = x3::space;

BOOST_SPIRIT_INSTANTIATE( decltype(type),             iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(variable_name),    iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(quoted1_string),   iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(quoted2_string),   iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(single_variable_name),   iterator_type, context_type )

BOOST_SPIRIT_INSTANTIATE( decltype(is_required), iterator_type, context_type )
} // namespace ix3::text
