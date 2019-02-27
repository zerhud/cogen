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
	if(!part) part = opts.get_child_optional("defaults");
	if(!part) throw errors::error("no options for part"s + std::string(name));
	return *part;
}

boost::property_tree::ptree mg::options_view::target_data(std::string_view name) const
{
	auto part = opts.get_child_optional("target."s+std::string(name));
	if(part) return *part;
	return boost::property_tree::ptree{};
}

std::vector<std::string> mg::options_view::part_str_list(const std::string& path, std::string_view key, std::string_view name) const
{
	std::vector<std::string> ret;

	std::string pn(name.empty() ? part : name);
	std::string suffix = path.empty() ? path : "."s + path;

	auto child_list = opts.get_child_optional("gen."s+pn+suffix);
	for(auto& [k,v]:*child_list) if(k==key) ret.emplace_back(v.get_value<std::string>());

	if(ret.empty()) {
		child_list = opts.get_child_optional("defaults"s + suffix);
		for(auto& [k,v]:*child_list) if(k==key) ret.emplace_back(v.get_value<std::string>());
	}

	return ret;
}

std::optional<std::string> mg::options_view::part_str(const std::string& path, std::string_view name) const
{
	auto ret = opts.get_optional<std::string>("gen."s+std::string(name.empty()?part:name)+"."s+path);
	if(ret) return *ret;
	ret = opts.get_optional<std::string>("defaults."s+path);
	if(ret) return *ret;
	return std::nullopt;
}

std::optional<std::string> mg::options_view::target_str(const std::string& path, std::string_view name) const
{
	auto tkey = "target."s + std::string(name) + "."s + path;
	auto pkey = "gen."s + std::string(part) + "."s + tkey;
	auto part_val = opts.get_optional<std::string>(pkey);
	if(!part_val) part_val = opts.get_optional<std::string>(tkey);
	if(part_val) return *part_val;
	return std::nullopt;
}


std::string mg::options_view::naming() const
{
	return part_data().get("naming",opts.get("defaults.naming",""s));
}
