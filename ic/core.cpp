/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "core.hpp"

modegen::ic::core::core()
{
}

void modegen::ic::core::gen(std::shared_ptr<output> out, std::shared_ptr<configuration> config) const
{
	if(!out || !config)
		throw std::runtime_error("cannot generate output without configuration or output provider");
	auto parts = config->parts();
	for(auto& part:parts) {
		part->rename(config->naming(part->id()));
		if(config->split_versions(part->id()))
			part->split_versions();
		part->map_to(config->map_tmpl(part->id()));
		auto outs = part->outputs();
		for(auto& out:outs)
			out->gen(config->output_dir(), config->tmpl_information(part->id()));
	}
}

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

void modegen::ic::input::add(std::vector<std::shared_ptr<modegen::ic::input_node> > list)
{
	add(true, std::move(list));
}

void modegen::ic::input::add(
        modegen::ic::input_node* par,
        std::vector<std::shared_ptr<modegen::ic::input_node> > list)
{
	if(!node_exists(par))
		throw std::runtime_error("cannot find parent node");
	add_to_parent(par, to_pointers(list));
	add(false, std::move(list));
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

void modegen::ic::input::add_to_parent(modegen::ic::input_node* par, std::vector<modegen::ic::input_node*> list)
{
	for(auto& e:edges) {
		if(e.parent==par) {
			e.children.insert(e.children.end(), list.begin(), list.end());
			return;
		}
	}
	edges.emplace_back(edge{par, std::move(list)});
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
}
