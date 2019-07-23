/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include <string>
#include "common.hpp"
#include "parser/interface/modegen.hpp"

namespace modegen::generation::interface {

enum class name_conversion{underscore, camel_case, title_case, as_is};
std::string_view to_string(name_conversion c);
name_conversion from_string(std::string_view n);

class naming {
public:
	explicit naming(name_conversion c);
	explicit naming(std::string_view sn);
	std::vector<parser::interface::module>& operator () (std::vector<parser::interface::module>& mods) const ;

	static std::vector<std::string> split_name(const std::string& name);
	static std::string convert(const std::string& name, name_conversion c);
private:
	name_conversion conver;
};

} // namespace modegen::generation::interface
