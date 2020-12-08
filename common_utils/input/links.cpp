/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "links.hpp"

using gen_utils::links_manager;
using gen_utils::node_links_info;

links_manager::links_manager(std::pmr::vector<const gen_utils::tree*> src)
    : src_list(std::move(src))
{
	for(auto& t:src_list) rscan(*t, t->root());
}

void links_manager::rscan(const tree& src, const data_node& dn)
{
	for(name_t& link:dn.required_links()) {
		auto to = search_for(link);
		if(to) storage.emplace_back(
		            link_info{
		                node_links_info{src.data_id(), &dn},
		                std::move(*to)} );
	}

	for(auto& c:src.children(dn)) rscan(src, *c);
}

std::optional<node_links_info> links_manager::search_for(name_t& n) const
{
	assert(!n.empty());
	for(auto& src:src_list) {
		auto found = search_for(*src, src->root(), n);
		if(found) return found;
	}
	return std::nullopt;
}

std::optional<node_links_info> links_manager::search_for(
        const tree& src, const data_node& par, name_t& n) const
{
	assert(!n.empty());
	if(par.name()==n.front() && n.size()==1)
		return node_links_info{src.data_id(), &par};
	name_t nn(++(n.begin()),n.end());
	if(!nn.empty()) for(auto& c:src.children(par)) {
		auto result = search_for(src, *c, nn);
		if(result) return result;
	}
	return std::nullopt;
}

std::pmr::vector<node_links_info> links_manager::links(const gen_utils::data_node& from) const
{
	std::pmr::vector<node_links_info> ret;
	for(auto& info:storage)
		if(info.from.node == &from)
			ret.emplace_back(info.to);
	return ret;
}
