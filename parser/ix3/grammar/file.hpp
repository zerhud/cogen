/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once


#include <boost/spirit/home/x3.hpp>
#include "fusion/file.hpp"

namespace ix3::text {

namespace x3 = boost::spirit::x3;

class module_class;
class include_st_class;
class file_content_class;

const x3::rule<module_class, ast::module> module = "module";
const x3::rule<include_st_class, ast::include_st> include_st = "include_st";
const x3::rule<file_content_class, ast::file_content> file_content = "file_content";

BOOST_SPIRIT_DECLARE(decltype(module))
BOOST_SPIRIT_DECLARE(decltype(include_st))
BOOST_SPIRIT_DECLARE(decltype(file_content))

} // namespace ix3::text

