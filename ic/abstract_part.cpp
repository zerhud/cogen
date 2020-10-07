/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "abstract_part.hpp"
#include "algos/split.hpp"

modegen::ic::abstract::part::part(
          std::size_t id
        , std::string name_in_config
        , std::shared_ptr<modegen::ic::input> all_input)
    : id_(id)
    , name_(std::move(name_in_config))
    , data_(std::move(all_input))
{
	if(!data_) throw std::runtime_error("cannot create part wihtout data");
}

std::size_t modegen::ic::abstract::part::id() const
{
	return id_;
}

std::string modegen::ic::abstract::part::name() const
{
	return name_;
}

std::vector<std::shared_ptr<modegen::ic::output>>
modegen::ic::abstract::part::outputs() const
{
	return {};
}

void modegen::ic::abstract::part::rename(gen_utils::name_conversion to)
{
	//auto renamer = [to](const std::string& n){return gen_utils::convert(n, to);};
}

void modegen::ic::abstract::part::split_versions()
{
}

void modegen::ic::abstract::part::map_to(std::string_view tmpl)
{

}
