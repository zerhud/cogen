/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "input.hpp"

#include <cassert>
#include <algorithm>
#include <ranges>

using namespace std::literals;

std::vector<std::shared_ptr<modegen::ic::input_node> > modegen::ic::input::all() const
{
	return nodes;
}

std::vector<modegen::ic::input_node*> modegen::ic::input::children(const modegen::ic::input_node* n) const
{
	for(auto& edge:edges) {
		assert(node_exists(edge.parent));
		if(edge.parent==n)
			return edge.children;
	}
	if(n==nullptr) return roots;
	return {};
}

void modegen::ic::input::add(std::vector<std::shared_ptr<modegen::ic::input_node>> list)
{
	for(auto& r:list) {
		if(!r)
			throw std::runtime_error("cannot add nullptr"s);
		if(r->level()!=0)
			throw std::runtime_error("root level must to be zero"s);
		if(node_exists(r.get())) {
			auto pos = std::find(roots.begin(),roots.end(), r.get());
			std::ranges::find(roots, r.get());
		}
	}
	add(true, std::move(list));
}

void modegen::ic::input::add(
        modegen::ic::input_node* par,
        std::vector<std::shared_ptr<modegen::ic::input_node>> list)
{
	if(!par) throw std::runtime_error("cannot add to a nullptr"s);
	if(!node_exists(par))
		throw std::runtime_error("cannot find parent node"s);
	for(auto& child:list) {
		if(!child)
			throw std::runtime_error("cannot add nullptr"s);
		if(child->level() <= par->level())
			throw std::runtime_error("child level is less or equal pareet"s);
	}

	add_to_parent(par, to_pointers(list));
	add(false, std::move(list));

	assert(check_tree_levels().empty());
}

bool modegen::ic::input::node_exists(modegen::ic::input_node* node) const
{
	auto pos = std::find_if(
	            nodes.begin(), nodes.end(),
	            [node](auto& n){return n.get()==node;});
	return pos != nodes.end();
}

std::vector<modegen::ic::input_node*> modegen::ic::input::to_pointers(
        const std::vector<std::shared_ptr<modegen::ic::input_node> >& list) const
{
	std::vector<input_node*> pointers;
	for(auto& n:list) pointers.emplace_back(n.get());
	return pointers;
}

void modegen::ic::input::add_to_parent(
        modegen::ic::input_node* par, std::vector<modegen::ic::input_node*> list)
{
	for(auto& e:edges) {
		if(e.parent==par) {
			e.children.insert(e.children.end(), list.begin(), list.end());
			return;
		}
	}
	edges.emplace_back(edge{par, std::move(list)});
}

std::string modegen::ic::input::check_tree_levels() const
{
	for(auto& root:roots)
		if(root->level() != 0)
			return "root level is " + std::to_string(root->level());
	for(auto& e:edges) {
		for(auto& child:e.children)
			if(child->level() <= e.parent->level()) {
				return
				          std::to_string(e.parent->level())
				        + ">="s
				        + std::to_string(child->level());
			}
	}
	return ""s;
}

void modegen::ic::input::add(bool is_root, std::vector<std::shared_ptr<modegen::ic::input_node> > list)
{
	for(auto& n:list) if(!n) throw std::runtime_error("cannot add nullptr");
	if(is_root) for(auto& n:list) roots.emplace_back(n.get());
	for(auto& n:list) {
		if(node_exists(n.get()))
			throw std::runtime_error("node already exists");
		nodes.emplace_back(std::move(n));
	}
	assert(check_tree_levels().empty());
}
