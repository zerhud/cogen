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
#include <ostream>
#include <memory_resource>
#include "naming.hpp"

namespace gen_utils {

enum class compiler { cpp } ;
enum class tree_compare_result { not_comparable, none, only_root, partial, total };

class tree;
class input;
class data_node;
using node_ptr = std::shared_ptr<data_node>;
using name_t = std::pmr::vector<std::pmr::string>;

class imports_manager;

struct import_file {
	bool sys;
	std::pmr::string name;
};

struct node_pointer {
	node_ptr node;
	const tree* owner;
};

struct compilation_config final {
	compiler name=compiler::cpp;
	std::pmr::vector<name_conversion> naming{name_conversion::as_is};
};

struct compilation_context final {
	compilation_config cfg;
	const imports_manager* links = nullptr;
	node_ptr linked_to = nullptr;
};

inline bool operator == (const node_pointer& l, const node_pointer& r) {
	return l.node == r.node && l.owner == r.owner;
}
inline bool operator < (const node_pointer& l, const node_pointer& r) {
	return l.node < r.node || l.owner < r.owner;
}

inline bool operator == (const import_file& l, const import_file& r) {
	return l.sys == r.sys && l.name == r.name;
}
inline bool operator < (const import_file& l, const import_file& r) {
	return l.sys < r.sys || l.name < r.name;
}

inline std::ostream& operator << (std::ostream& out, const import_file& f) {
	return out << (f.sys ? '<' : '"') << f.name << (f.sys ? '>' : '"');
}

} // namespace gen_utils
