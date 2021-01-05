/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <memory>
#include <vector>
#include <memory_resource>

namespace gen_utils {

enum class compiler { cpp } ;
enum class tree_compare_result { not_comparable, none, only_root, partial, total };

class tree;
class input;
class data_node;
using node_ptr = std::shared_ptr<data_node>;
using name_t = std::pmr::vector<std::pmr::string>;

struct node_pointer {
	node_ptr node;
	const tree* owner;
};

inline bool operator == (const node_pointer& l, const node_pointer& r) {
	return l.node == r.node && l.owner == r.owner;
}

} // namespace gen_utils
