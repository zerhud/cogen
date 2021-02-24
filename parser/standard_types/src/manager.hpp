/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "utils/tree.hpp"

namespace std_types {

class manager : public gen_utils::dsl_manager {
public:
	std::string_view id() const override ;
	boost::json::value to_json(
	        const gen_utils::compilation_context& cfg,
	        const gen_utils::tree& container) const override ;
};

} // namespace std_types

