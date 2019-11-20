/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "functions.hpp"
#include "common.hpp"
#include "../parse.hpp"

namespace ix3::text {

BOOST_SPIRIT_INSTANTIATE( decltype(is_static),          iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(is_mutable),         iterator_type, context_type )

BOOST_SPIRIT_INSTANTIATE( decltype(function),           iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(constructor),        iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(function_parameter), iterator_type, context_type )

} // namespace ix3::text

