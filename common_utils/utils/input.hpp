/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <functional>
#include "tree.hpp"

namespace gen_utils {

class input final {
	std::pmr::vector<tree> storage;
	compilation_config cur_conf;
public:
	using mod_fnc = std::function<std::pmr::vector<tree>(const tree&)>;
	input() =default ;
	~input() noexcept =default;

	input& add(input&& other);
	input& add(tree data);

	[[nodiscard]] std::pmr::vector<const tree*> select(
			std::string_view id) const ;

	[[nodiscard]] std::pmr::vector<const tree*> all() const ;

	[[nodiscard]]
	tree_compare_result match_with(const input& other) const ;

	compilation_config& conf() ;
	const compilation_config& conf() const ;

	input modify(const mod_fnc& mod) const ;
};


} // namespace gen_utils
