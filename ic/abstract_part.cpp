/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "abstract_part.hpp"

#include <common_utils/input/map_to.hpp>

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
	if(!compiled_.empty())
		throw std::runtime_error("cannot call map_to twice");
	gen_utils::map_to mapper;
	for(auto& t:data_.all()) {
		auto result = mapper(std::pmr::string(tmpl), *t);
		for(auto& [name, value]:result)
			compiled_[name].add(std::move(value));
	}
}

std::pmr::vector<modegen::ic::map_result>
        modegen::ic::abstract::part::compiled_input() const
{
	std::pmr::vector<modegen::ic::map_result> ret;
	for(auto& [tmpl, data]:compiled_)
		ret.emplace_back(map_result{tmpl,data});
	return ret;
}
