/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <queue>
#include <boost/property_tree/ptree.hpp>

namespace boost { namespace property_tree {

template<class Key=ptree::key_type, class Data=ptree::data_type, class KeyCompare=ptree::key_compare>
class merger {
	typedef merger<Key, Data, KeyCompare> self_type;
	typedef basic_ptree<Key, Data, KeyCompare> ptree_type;
public:
	merger(ptree_type basic=ptree{})
	    : result(std::move(basic))
	{
	}

	operator ptree_type() const
	{
		return result;
	}

	self_type& put(ptree_type data)
	{
		begin_merge(std::move(data));
		while(!values.empty()) {
			auto [key, cur_data] = begin_iteration();
			for(auto& child:cur_data) {
				auto [leaf_key, is_leaf] = iterate_child(child, key);
				if(is_leaf) result.put( leaf_key, child.second.data());
			}
		}

		assert(keys.empty());
		assert(values.empty());
		return *this;
	}

	self_type& add(ptree_type data)
	{
		begin_merge(std::move(data));
		while(!values.empty()) {
			auto [key, cur_data] = begin_iteration();
			for(auto& child:cur_data) {
				auto [leaf_key, is_leaf] = iterate_child(child, key);
				if(is_leaf) result.add( leaf_key, child.second.data());
			}
		}

		assert(keys.empty());
		assert(values.empty());
		return *this;
	}

	self_type& error(ptree_type data)
	{
		begin_merge(std::move(data));
		while(!values.empty()) {
			auto [key, cur_data] = begin_iteration();
			for(auto& child:cur_data) {
				auto [leaf_key, is_leaf] = iterate_child(child, key);
				if(is_leaf) {
					const bool exists = result.get_child_optional(leaf_key) != boost::none;
					if(exists) throw ptree_bad_path("already exists", leaf_key);
					result.put( leaf_key, child.second.data());
				}
			}
		}
		assert(keys.empty());
		assert(values.empty());
		return *this;
	}

private:
	void begin_merge(ptree_type data)
	{
		assert(keys.empty());
		assert(values.empty());
		values.push(std::move(data));
	}

	auto begin_iteration()
	{
		typename ptree_type::path_type key;
		if(!keys.empty()) {
			key = std::move(keys.front());
			keys.pop();
		}

		assert(!values.empty());
		auto data = std::move(values.front());
		values.pop();

		return std::make_tuple(std::move(key), std::move(data));
	}

	template<typename Child, typename Path>
	auto iterate_child(const Child& child, const Path& key)
	{
		auto [lkey, is_leaf] = child_data(child, key);
		if(!is_leaf) {
			keys.push(lkey);
			values.push(child.second);
		}

		return std::make_tuple(lkey, is_leaf);
	}

	template<typename Child, typename Path>
	auto child_data(const Child& child, const Path& key)
	{
		Path child_key = child.first;
		Path leaf_key = (key.empty() ? child_key : key / child_key);
		const bool is_leaf = child.second.empty();
		return std::make_tuple(leaf_key, is_leaf);
	}

	ptree_type result;
	std::queue<typename ptree_type::path_type> keys;
	std::queue<ptree_type> values;
};

}} // namespace boost::property_tree
