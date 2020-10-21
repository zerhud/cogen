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
	cur_data.swap(result);
	for(auto& item:cur_data) {
		auto val_list = item.second.var_value_list(name);
		assert(!val_list.empty());
		for(auto& val:val_list) {
			auto tmpl = replace(item.first, name, val);
			if(!tmpl) {
				result.emplace(std::move(item));
				continue;
			}
			result.emplace(*tmpl, copy_for(item.second, name, val));
		}
	}
}

gen_utils::tree gen_utils::map_to::copy_for(
		tree& data, std::string_view name, std::string_view value) const
{
	return data.copy([&name,&value](const data_node& n){
		auto var = n.node_var();
		if(!var || var->name!=name) return true;
		return var->value==value;
	});
}

gen_utils::map_to::result_t gen_utils::map_to::operator()
(std::pmr::string tmpl, const tree& data)
{
	auto variables = data.var_name_list();
	result.emplace(tmpl, data);
	for(auto& var:variables) {
		make_for_name(var);
	}
	return result;
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
