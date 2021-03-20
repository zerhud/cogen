/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "entity.hpp"
#include "../parse.hpp"

namespace ix3::text {

BOOST_SPIRIT_INSTANTIATE( decltype(record),       iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(interface),    iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(enumeration),  iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(record_item),  iterator_type, context_type )
BOOST_SPIRIT_INSTANTIATE( decltype(enum_element), iterator_type, context_type )

} // namespace ix3::text

