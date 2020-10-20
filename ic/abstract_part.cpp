/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "abstract_part.hpp"

using namespace std::literals;

modegen::ic::abstract::part::part(
          std::uint64_t id
        , std::string name_in_config
        , modegen::ic::input all_input)
    : id_(id)
    , name_(std::move(name_in_config))
    , data_(std::move(all_input))
{
	//if(!data_) throw std::runtime_error("cannot create part without data");
}

std::uint64_t modegen::ic::abstract::part::id() const
{
	return id_;
}

std::string_view modegen::ic::abstract::part::name() const
{
	return name_;
}

void modegen::ic::abstract::part::rename(gen_utils::name_conversion to)
{
	//auto rename_doer = [to](const std::string& n){return gen_utils::convert(n, to);};
}

void modegen::ic::abstract::part::split_versions()
{
}

void modegen::ic::abstract::part::map_to(std::string_view tmpl)
{
	for(auto& d:data_.all()) {
		auto cur = create_input(std::pmr::string(tmpl), d);
		for(auto& c:cur) compiled_.emplace_back(std::move(c));
	}
}

std::pmr::vector<modegen::ic::map_result>
        modegen::ic::abstract::part::create_input(
		  std::pmr::string tmpl
		, const gen_utils::tree* data) const
{
	std::pmr::vector<modegen::ic::map_result> ret;
	auto var_list = data->var_name_list();
	for(auto& var:var_list) {
		auto val_list = data->var_value_list(var);
		for(auto& val:val_list) {
			auto cur_tmpl = replace(tmpl, var, val);
			if(!cur_tmpl) continue;
			auto checker = [&val,&var](const gen_utils::data_node& n){
				auto node_var = n.node_var();
				if(!node_var || node_var->name!=var) return true;
				return node_var->name==var && node_var->value == val;
			};
			auto cur_data = data->copy(checker);
			auto next_result = create_input(*cur_tmpl, &cur_data);
			if(next_result.empty()) {
				auto& r = ret.emplace_back();
				r.map = *cur_tmpl;
				r.data.add(std::move(cur_data));
			} else {
				for(auto& v:next_result)
					ret.emplace_back(std::move(v));
			}
		}
	}
	return ret;
}

std::optional<std::pmr::string> modegen::ic::abstract::part::replace(
		std::string_view tmpl, std::string_view name, std::string_view value) const
{
	std::optional<std::pmr::string> ret;
	auto pos = tmpl.find("${" + std::pmr::string(name) + "}");
	if(pos==std::string_view::npos) return ret;
	ret.emplace(tmpl).replace(pos, 3+name.size(), value);
	return ret;
}

std::pmr::vector<modegen::ic::map_result>
        modegen::ic::abstract::part::compiled_input() const
{
	return compiled_;
}
