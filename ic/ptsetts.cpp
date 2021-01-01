/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "ptsetts.hpp"
#include <iostream>

using mdg::ic::ptsetts;
using namespace std::literals;

ptsetts::ptsetts(boost::property_tree::ptree s)
	: setts(std::move(s))
{
}

std::pmr::vector<std::pmr::string> ptsetts::parts() const
{
	std::pmr::vector<std::pmr::string> ret;
	for(auto& p:setts.get_child("part"))
		ret.emplace_back(p.first);
	return ret;
}

mdg::ic::gen_config ptsetts::part_setts(std::string_view p) const
{
	gen_config ret;
	auto path = "part."s+std::string(p);
	ret.map_tmpl = setts.get<std::pmr::string>(path+".file"s);
	ret.tmpl_file = setts.get<std::pmr::string>(path+".tmpl"s);
	conf_links(path, ret);
	return ret;
}

void ptsetts::conf_links(const std::string& path, gen_config& cfg) const
{
	cfg.links.clear();
	auto incs = setts.get_child_optional(path);
	if(incs) for(auto& ip:*incs) {
		if(ip.first == "inc_part") // cannot access the last key
			cfg.links.emplace_back(ip.second.get_value<std::string>());
	}
}
