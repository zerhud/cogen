/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "declarations.hpp"
#include "parser/interface/loader.hpp"

namespace modegen::generation {

class cpp_generator {
public:
	nlohmann::json jsoned_data(const output_info& outputs) const ;
private:
	struct inc_info {
		inc_info(std::string n, bool s) : name(std::move(n)), sys(s) {}
		std::string name;
		bool sys;
	};

	std::vector<inc_info> includes(const std::vector<std::string> sys, const options::view& opts) const ;
	std::string solve_part_include(const std::string& part, const options::view& opts) const ;
	void add_extra_info(const options::view& opts, nlohmann::json& cdata) const ;
	void add_extra_namespaces(const options::view& opts, nlohmann::json& cdata) const ;
	void set_constructors_prefix(const options::view& opts, nlohmann::json& cdata) const ;
};

} // namespace modegen::generation
