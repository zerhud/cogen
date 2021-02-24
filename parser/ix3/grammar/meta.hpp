/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once


#include <boost/spirit/home/x3.hpp>
#include "fusion/meta.hpp"

namespace ix3::text {

namespace x3 = boost::spirit::x3;

class meta_set_class;
class meat_oapi_class;
class meta_version_class;
class meta_depricated_class;
class meta_documentation_class;

const x3::rule<meta_set_class, std::vector<ast::meta::set::parameter_t>> meta_set = "meta_set";
const x3::rule<meta_version_class, ast::meta::oapi> meta_oapi = "meta_oapi";
const x3::rule<meta_version_class, ast::meta::version> meta_version = "meta_version";
const x3::rule<meta_depricated_class, ast::meta::depricated> meta_depricated = "meta_depricated";
const x3::rule<meta_documentation_class, ast::meta::documentation> meta_documentation = "meta_documentation";

BOOST_SPIRIT_DECLARE(decltype(meta_set))
BOOST_SPIRIT_DECLARE(decltype(meta_oapi))
BOOST_SPIRIT_DECLARE(decltype(meta_version))
BOOST_SPIRIT_DECLARE(decltype(meta_depricated))
BOOST_SPIRIT_DECLARE(decltype(meta_documentation))

} // namespace ix3::text
