/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "file_data.hpp"
#include "errors.h"

using namespace std::literals;
namespace mg = modegen::generation;

mg::options_view::options_view(const boost::property_tree::ptree& o, std::string_view p)
	: opts(o)
	, part(p)
{
}

std::string_view mg::options_view::part_name() const
{
	return part;
}

boost::property_tree::ptree mg::options_view::part_data() const
{
	return part_data(part);
}

const boost::property_tree::ptree& mg::options_view::all() const
{
	return opts;
}


boost::property_tree::ptree mg::options_view::part_data(std::string_view name) const
{
	auto part = opts.get_child_optional("gen."s+std::string(name));
	if(!part) throw errors::error("no options for part"s + std::string(name));
	return *part;
}

boost::property_tree::ptree mg::options_view::target_data(std::string_view name) const
{
	auto part = opts.get_child_optional("target."s+std::string(name));
	if(part) return *part;
	return boost::property_tree::ptree{};
}

std::string mg::options_view::naming() const
{
	return part_data().get("naming",opts.get("defaults.naming",""s));
}
