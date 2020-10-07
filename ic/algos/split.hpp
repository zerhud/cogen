/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "../core.hpp"


namespace modegen::ic::algos {

class split final {
	const input_node* main_root;
	std::vector<const input_node*> nodes;
	void add_to_root(const input_node* node);
	void clone_exactly_version_root(std::optional<std::uint64_t> ver);
public:
	split(const input_node* root);
	void operator ()(const input_node* node);
};

} // namespace modegen::ic::algos
