/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "tree.hpp"

#include <cassert>
#include <stdexcept>

using namespace gen_utils;
using namespace std::literals;

tree::tree(node_ptr root, std::pmr::string id_) : id(std::move(id_))
{
	if(!root) throw std::runtime_error("cannot create input tree without a root");
	if(!root->version().has_value())
		throw std::runtime_error("root must have a version");
	root_ver = *root->version();
	store.emplace_back(std::move(root));
}

std::pmr::string tree::data_id() const
{
	return id;
}

const data_node& tree::root() const
{
	assert(!store.empty());
	return *store.front();
}

void tree::add(const data_node &par, node_ptr child)
{
	if(!node_exists(&par))
		throw std::runtime_error("no such node found"s);
	if(node_exists(child.get()))
		throw std::runtime_error("child already exists in this tree"s);
	if(child->version() && *child->version() < root_version())
		throw std::runtime_error("child version is less then root version"s);
	store.emplace_back(create_link(&par, std::move(child)));
}

node_ptr tree::create_link(const data_node *p, node_ptr c)
{
	for(auto& e:edges)
		if(e.parent==p)
			return e.children.emplace_back(std::move(c));
	return edges.emplace_back(edge{p, {std::move(c)}}).children[0];
}

std::pmr::vector<node_ptr> tree::children(const data_node& par) const
{
	if(!node_exists(&par))
		throw std::runtime_error("no such node found"s);
	for(auto& e:edges) if(e.parent==&par) return e.children;
	return {};
}

bool tree::node_exists(const data_node *n) const
{
	auto pos = std::find_if(
	            store.begin(), store.end(),
	            [n](auto& s){return s.get()==n;});
	return pos != store.end();
}

std::uint64_t tree::root_version() const
{
	return root_ver;
}

void tree::root_version(std::uint64_t v)
{
	root_ver = v;
}

std::uint64_t tree::next_min_version() const
{
	auto is_a_less = [](auto& a, auto& b){
		if(a->version() && b->version())
			return *a->version() < b->version();
		return a->version().has_value();
	};
	auto pos = std::min_element(++(store.begin()), store.end(), is_a_less);
	if(pos==store.end()) return root_ver;
	return (*pos)->version().value_or(root_ver);
}

std::pmr::vector<std::pmr::string> tree::var_name_list() const
{
	std::pmr::vector<std::pmr::string> ret;
	auto emp_unique = [&ret](auto&& v){
		auto pos = std::find(ret.begin(), ret.end(), v);
		if(pos==ret.end()) ret.emplace_back(v);
	};
	for(auto& n:store) if(n->node_var()) emp_unique(n->node_var()->name);
	return ret;
}

std::pmr::vector<std::pmr::string> tree::var_value_list(std::string_view name) const
{
	std::pmr::vector<std::pmr::string> ret;
	for(auto& n:store) {
		auto nv = n->node_var();
		if(nv && nv->name == name)
			ret.emplace_back(std::move(nv->value));
	}
	return ret;
}

tree tree::copy(const tree::copy_condition& cond) const
{
	if(!cond) return *this;

	tree ret(store[0], data_id());
	ret.root_ver = root_ver;
	for(auto& e:edges) if(cond(*e.parent)) {
		auto& ce = ret.edges.emplace_back(edge{e.parent, {}});
		for(auto& c:e.children) if(cond(*c)) {
			ret.store.emplace_back(c);
			ce.children.emplace_back(c);
		}
	}

	return ret;
}
