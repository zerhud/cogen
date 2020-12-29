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

void ptsetts::part_setts(std::string_view p, modegen::ic::gen_context& ctx) const
{
	auto path = "part."s+std::string(p);
	ctx.map_tmpl = setts.get<std::pmr::string>(path+".file"s);
	ctx.tmpl_file = setts.get<std::pmr::string>(path+".tmpl"s);
	conf_links(path, ctx);
}

void ptsetts::conf_links(const std::string& path, modegen::ic::gen_context& ctx) const
{
	ctx.links.clear();
	auto incs = setts.get_child_optional(path);
	if(incs) for(auto& ip:*incs) {
		if(ip.first == "inc_part") // cannot access the last key
			ctx.links.emplace_back(ip.second.get_value<std::string>());
	}
}
