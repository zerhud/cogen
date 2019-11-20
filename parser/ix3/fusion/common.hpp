/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ast/common.hpp"

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(ix3::ast::type,      name, sub_types)
BOOST_FUSION_ADAPT_STRUCT(ix3::ast::meta::set, cnt)

BOOST_FUSION_ADAPT_STRUCT(ix3::ast::meta::version,       major_v, minor_v)
BOOST_FUSION_ADAPT_STRUCT(ix3::ast::meta::depricated,    since, message )
BOOST_FUSION_ADAPT_STRUCT(ix3::ast::meta::documentation, body)

