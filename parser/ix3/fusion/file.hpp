/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ast/file.hpp"

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(ix3::ast::include_st, path)
BOOST_FUSION_ADAPT_STRUCT(ix3::ast::module, meta_params, name, version, content)
BOOST_FUSION_ADAPT_STRUCT(ix3::ast::file_content, includes, modules)

