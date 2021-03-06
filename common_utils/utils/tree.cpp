/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "tree.hpp"

#include <cassert>
#include <stdexcept>
#include <boost/json/src.hpp>

using namespace gen_utils;
using namespace std::literals;

tree::tree(node_ptr root, std::shared_ptr<dsl_manager> dm) : dmanager(std::move(dm))
{
	if(!dmanager) throw std::runtime_error("cannot create input tree without dsl manager");
	if(!root) throw std::runtime_error("cannot create input tree without a root");
	if(!root->version().has_value())
		throw std::runtime_error("root must have a version");
	root_ver = *store.emplace_back(std::move(root))->version();
}

boost::json::value tree::to_json(const compilation_context& ctx) const
{
	assert(dmanager);
	return dmanager->to_json(ctx, *this);
}

std::string_view tree::data_id() const
{
	assert(dmanager);
	return dmanager->id();
}

const data_node& tree::root() const
{
	assert(!store.empty());
	return *store.front();
}

tree& tree::add(const data_node &par, node_ptr child)
{
	if(!node_exists(&par))
		throw std::runtime_error("no such node found"s);
	if(node_exists(child.get()))
		throw std::runtime_error("child already exists in this tree"s);
	if(child->version() && *child->version() < root_version())
		throw std::runtime_error("child version is less then root version"s);
	store.emplace_back(create_link(&par, std::move(child)));
	return *this;
}

node_ptr tree::create_link(const data_node *p, node_ptr c)
{
	for(auto& e:edges)
		if(e.parent==p)
			return e.children.emplace_back(std::move(c));
	return edges.emplace_back(edge{p, {std::move(c)}}).children[0];
}

node_ptr tree::create_link_if_no(const data_node *p, node_ptr c)
{
	edge* ie = search_edge(*p);
	if(!ie) return create_link(p, std::move(c));
	auto& ch = ie->children;
	auto ch_pos = std::find(ch.begin(),ch.end(), c);
	if(ch_pos == ch.end())
		return store.emplace_back(create_link(p, std::move(c)));
	return *ch_pos;
}

std::pmr::vector<node_ptr> tree::children(const data_node& par) const
{
	if(!node_exists(&par))
		throw std::runtime_error("no such node found"s);
	for(auto& e:edges) if(e.parent==&par) return e.children;
	return {};
}

tree_compare_result tree::contains(const tree& other) const
{
	using tcr = tree_compare_result;
	if(data_id() != other.data_id())
		return tcr::not_comparable;
	if(&root() != &other.root())
		return tcr::none;
	std::size_t match_count = 0;
	for(auto& sn:store)
		if(other.node_exists(sn.get())) ++match_count;
	return match_count == 1
	     ? (store.size() == 1 ? tcr::total : tcr::only_root)
	     : (match_count == other.store.size() ? tcr::total : tcr::partial)
	     ;
}

std::pmr::vector<node_ptr> tree::search(name_t n) const
{
	std::pmr::vector<node_ptr> ret;
	if(n.empty()) return ret;
	return search(root(), std::move(n));
}

std::pmr::vector<node_ptr> tree::search(const data_node& par, name_t n) const
{
	std::pmr::vector<node_ptr> ret;
	assert(!n.empty());
	const edge* ce = search_edge(par);
	if(ce) for(auto& child:ce->children) {
		if(child->name() == n[0]) {
			if(n.size()==1) ret.emplace_back(child);
			else {
				auto fnd = search(*child, name_t{++n.begin(),n.end()});
				ret.insert(ret.end(), fnd.begin(), fnd.end());
			}
		}
		auto fnd = search(*child, n);
		ret.insert(ret.end(), fnd.begin(), fnd.end());
	}
	return ret;
}

tree::edge* tree::search_edge(const data_node& par)
{
	for(auto& e:edges) if(e.parent == &par) return &e;
	return nullptr;
}

const tree::edge* tree::search_edge(const data_node& par) const
{
	return const_cast<tree*>(this)->search_edge(par);
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
	std::pmr::vector<std::uint64_t> vers;
	for(auto& n:store) if(n->version()) vers.emplace_back(*n->version());
	std::ranges::sort(vers);
	for(auto& v:vers) if(root_ver < v) return v;
	return root_ver;
}

const data_node* tree::parent(const data_node& child, const copy_condition cond) const
{
	if(&child == &root()) return nullptr;
	for(auto& e:edges) {
		auto cpos = std::find_if(
				e.children.begin(), e.children.end(),
				[&child](const node_ptr& c) {
					return c.get() == &child; }
				);
		if(cpos == e.children.end()) continue;
		if( cond ) return cond(*e.parent)
			? e.parent
			: parent(*e.parent, cond);
		return e.parent;
	}
	throw std::runtime_error("no such node exists");
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

std::optional<tree> tree::copy_if(const tree::copy_condition& cond) const
{
	std::optional<tree> ret ;

	if( !cond || !cond(root()) ) return ret ;

	ret.emplace(store[0], dmanager);
	ret->root_ver = root_ver;
	for(auto& e:edges) if(ret->node_exists(e.parent) && cond(*e.parent)) {
		auto& ce = ret->edges.emplace_back(edge{e.parent, {}});
		for(auto& c:e.children) if(cond(*c)) {
			ret->store.emplace_back(c);
			ce.children.emplace_back(c);
		}
	}

	return ret;
}

void tree::merge(tree other)
{
	if(other.data_id() != data_id())
		throw std::runtime_error("cannot merge different trees"s);
	if(&other.root() != &root())
		throw std::runtime_error("cannot merge trees with defferent roots"s);
	for(auto& oe:other.edges) {
		auto ie = search_edge(*oe.parent);
		if(!ie) {
			assert(node_exists(oe.parent));
			edges.emplace_back(oe);
			for(auto& c:oe.children)
				store.emplace_back(c);
		} else {
			for(auto& child:oe.children)
				create_link_if_no(oe.parent, child);
		}
	}
}
