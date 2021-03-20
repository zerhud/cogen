/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include <string>
#include <memory_resource>

namespace gen_utils {

	enum class name_conversion{underscore, camel_case, title_case, as_is};
	std::vector<std::pmr::string> split_name(const std::pmr::string& name);
	std::pmr::string convert(std::string_view name, name_conversion c);

	std::string_view to_string(name_conversion v);
	void from_string(std::string_view val, name_conversion& c);

} // namespace gen_utils

