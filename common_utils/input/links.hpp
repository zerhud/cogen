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
#include "tree.hpp"

namespace gen_utils {

struct node_links_info {
	/// refers to dsl_manager::id
	std::string_view dsl_id;
	/// refers to tree node
	const data_node* node;
};

class links_manager final {
	struct link_info {
		node_links_info from;
		node_links_info to;
	};
	std::vector<link_info> storage;
	std::pmr::vector<const tree*> src_list;

	std::optional<node_links_info> serach_for(
	        const tree& src, const data_node& par, name_t& n) const ;
	std::optional<node_links_info> serach_for(name_t& n) const ;
	void rscan(const tree& src, const data_node& dn) ;
public:
	links_manager(std::pmr::vector<const tree*> src);
	std::pmr::vector<node_links_info> links(const data_node& from) const ;
};

} // namespace gen_utils
