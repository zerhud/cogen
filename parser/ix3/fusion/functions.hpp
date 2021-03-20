/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ast/functions.hpp"

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(ix3::ast::function_parameter, required, param_type, name)
BOOST_FUSION_ADAPT_STRUCT(ix3::ast::function, meta_params, is_static, return_type, name, params, is_mutable)
BOOST_FUSION_ADAPT_STRUCT(ix3::ast::constructor, meta_params, params)

