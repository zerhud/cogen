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
#include "common_utils/input/tree.hpp"

namespace mdg::ic {

class input final {
	std::pmr::vector<gen_utils::tree> storage;
public:
	input() =default ;
	~input() noexcept =default;

	void add(input&& other);
	void add(gen_utils::tree data);

	[[nodiscard]] std::pmr::vector<const gen_utils::tree*> select(
			std::string_view id) const ;

	[[nodiscard]] std::pmr::vector<const gen_utils::tree*> all() const ;

	[[nodiscard]]
	gen_utils::tree_compare_result match_with(const input& other) const ;
};


} // namespace modegen::ic
