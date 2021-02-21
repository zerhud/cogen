/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "utils/tree.hpp"

namespace builders {

class dsl_manager : public gen_utils::dsl_manager {
public:
	std::string_view id() const override ;
	boost::json::value to_json(
	        const gen_utils::compilation_context& ctx,
	        const gen_utils::tree& container) const override ;
};

} // namespace builders
