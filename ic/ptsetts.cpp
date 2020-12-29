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

	ctx.links.clear();
	auto incs = setts.equal_range(path+".inc_part"s);
	for(;incs.first!=incs.second;++incs.first) {
		std::cout << incs.first->first << std::endl;
	}
	//auto incs = setts.get_value() .get_child_optional(path+".inc_part");
	//if(incs) for(auto& ip:*incs) {
	    //std::cout << ip.first << std::endl;
	    //ctx.links.emplace_back(ip.second.get_value<std::string>());
	//}
}
