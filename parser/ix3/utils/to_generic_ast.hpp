/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ix3/ast/file.hpp"
#include "common_utils/input/tree.hpp"

namespace ix3::utils {

class to_generic_ast {
public:
	gen_utils::tree operator()(std::pmr::vector<ast::module> mods);
};

} // namespace ix3::utils
