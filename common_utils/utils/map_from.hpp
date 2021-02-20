/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include "map_to.hpp"

namespace gen_utils {

class map_from {
public:
	typedef std::pmr::map<std::pmr::string,std::vector<std::pmr::string>> result_t;
	[[nodiscard]]  result_t operator()(
	        const map_to::result_t& mapped,
	        std::string_view tmpl,
	        const tree& src) ;
	[[nodiscard]]  result_t operator()(
	        const map_to::result_inputs_t& mapped,
	        std::string_view tmpl,
	        const input& src) ;
};

} // namespace gen_utils
