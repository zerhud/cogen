/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <map>
#include <string>
#include <memory_resource>

#include "tree.hpp"
#include "input.hpp"

namespace gen_utils {

class map_to {
public:
	typedef std::pmr::map<std::pmr::string, tree> result_t;
	typedef std::pmr::map<std::pmr::string, input> result_inputs_t;
	[[nodiscard]] result_t operator()(std::pmr::string tmpl, const tree& data);
	[[nodiscard]] result_inputs_t operator()(std::pmr::string tmpl, const input& data);
private:
	[[nodiscard]] std::optional<std::pmr::string> replace(
			std::string_view tmpl,
			std::string_view name,
			std::string_view value
			) const ;
	void make_for_name(std::string_view name) ;
	std::optional<tree> copy_for(tree& data, std::string_view name, std::string_view value) const ;
	void build_item(std::string_view name, result_t::value_type item);
	result_inputs_t map_to_input(
	        const result_inputs_t& src,
	        const tree& data,
	        const bool addi) ;

	result_t result;
};

} // namespace gen_utils
