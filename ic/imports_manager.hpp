/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "declarations.hpp"
#include "common_utils/input/declarations.hpp"

#include <map>

namespace mdg::ic {

struct import_info {
	gen_utils::node_pointer to;
	gen_utils::node_pointer from;
	std::pmr::string file;
	std::pmr::string cond;
};

class imports_manager final {
	std::pmr::map<std::pmr::string, const input*> all_input;
	std::pmr::vector<import_info> imports;
	std::pmr::map<const input*, std::pmr::vector<std::pmr::string>> matched;

	void scan_self_matched();
public:
	void build();
	imports_manager& add(const std::pmr::string& file, const input& data);
	std::pmr::vector<import_info> result_for(const input& file_data) const ;
	std::pmr::vector<std::pmr::string> self_matched(const input& file_data) const ;
};

} // namespace mdg::ic
