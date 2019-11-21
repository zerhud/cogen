/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ast/entity.hpp"

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(ix3::ast::record, meta_params, name, use_as_exception, members)
BOOST_FUSION_ADAPT_STRUCT(ix3::ast::interface, meta_params, name, realization_in_client, use_as_exception, constructors, mem_funcs)
BOOST_FUSION_ADAPT_STRUCT(ix3::ast::enumeration, meta_params, name, gen_io, use_bitmask, elements)
BOOST_FUSION_ADAPT_STRUCT(ix3::ast::record_item, meta_params, is_required, param_type, name)
BOOST_FUSION_ADAPT_STRUCT(ix3::ast::enum_element, name, io)

