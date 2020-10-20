/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <map>
#include <string>
#include <memory_resource>

#include "tree.hpp"

namespace gen_utils {

class map_to {
public:
	typedef std::map<std::pmr::string, tree> result_t;
	[[nodiscard]] result_t operator()(std::pmr::string tmpl, const tree& data);
private:
	[[nodiscard]] std::optional<std::pmr::string> replace(
			std::string_view tmpl,
			std::string_view name,
			std::string_view value
			) const ;
	void make_for_name(std::string_view name) ;
	tree copy_for(tree& data, std::string_view name, std::string_view value) const ;

	result_t result;
};

} // namespace gen_utils
