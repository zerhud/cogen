/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once


#include <boost/spirit/home/x3.hpp>
#include "fusion/entity.hpp"

namespace ix3::text {

namespace x3 = boost::spirit::x3;

class record_class;
class interface_class;
class enumeration_class;
class record_item_class;
class enum_element_class;

const x3::rule<record_class,      ast::record> record = "record";
const x3::rule<interface_class,   ast::interface> interface = "interface";
const x3::rule<enumeration_class, ast::enumeration> enumeration = "enumeration";
const x3::rule<record_item_class, ast::record_item> record_item = "record_item";
const x3::rule<enum_element_class, ast::enum_element> enum_element = "enum_element";

BOOST_SPIRIT_DECLARE(decltype(record))
BOOST_SPIRIT_DECLARE(decltype(interface))
BOOST_SPIRIT_DECLARE(decltype(enumeration))
BOOST_SPIRIT_DECLARE(decltype(record_item))
BOOST_SPIRIT_DECLARE(decltype(enum_element))

} // namespace ix3::text
