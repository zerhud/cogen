/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "map_to.hpp"

#include <cassert>

void gen_utils::map_to::make_for_name(std::string_view name)
{
	result_t cur_data;
	result.swap(cur_data);
	for(auto& item:cur_data) build_item(name, std::move(item));
}

void gen_utils::map_to::build_item(
		std::string_view name, result_t::value_type item)
{
	auto val_list = item.second.var_value_list(name);
	assert(!val_list.empty());
	for(auto& val:val_list) {
		auto tmpl = replace(item.first, name, val);
		if( tmpl ) {
			auto tree = copy_for(item.second, name, val);
			assert( tree) ;
			result.emplace(*tmpl, *tree);
		}
		else result.emplace(std::move(item));
	}
}

std::optional<gen_utils::tree> gen_utils::map_to::copy_for(
		tree& data, std::string_view name, std::string_view value) const
{
	return data.copy_if([&name, &value](const data_node &n) {
		auto var = n.node_var();
		if(!var || var->name!=name) return true;
		return var->value == value;
	});
}

gen_utils::map_to::result_t gen_utils::map_to::operator()
(std::pmr::string tmpl, const tree& data)
{
	result.clear();
	result.emplace(tmpl, data);
	for(auto& name:data.var_name_list())
		make_for_name(name);
	return result;
}

gen_utils::map_to::result_inputs_t gen_utils::map_to::operator()
(std::pmr::string tmpl, const input& data)
{
	result_inputs_t ret;
	auto all_data = data.all();
	while(!all_data.empty()) {
		if(ret.empty())
			ret = map_to_input({{tmpl,data}}, *all_data.back(), false);
		else ret = map_to_input(ret, *all_data.back(), true);
		all_data.pop_back();
	}
	return ret;
}

gen_utils::map_to::result_inputs_t gen_utils::map_to::map_to_input(
        const result_inputs_t& src, const tree& data, const bool addi)
{
	result_inputs_t cur;
	for(auto& [t,i]:src) {
		for(auto& [n,r]:(*this)(t, data)) {
			cur[n].add(std::move(r));
			if(addi) cur[n].add(input(i));
		}
	}
	return cur;
}

std::optional<std::pmr::string> gen_utils::map_to::replace(
		std::string_view tmpl,
		std::string_view name,
		std::string_view value
) const
{
	std::optional<std::pmr::string> ret;
	auto var_name = "${" + std::pmr::string(name) + "}";
	auto pos = tmpl.find(var_name);
	if(pos==std::string_view::npos) return ret;
	ret.emplace(tmpl).replace(pos, var_name.size(), value);
	for(pos=ret->find(var_name);pos!=std::pmr::string::npos;pos=ret->find(var_name))
		ret->replace(pos, var_name.size(), value);
	return ret;
}
