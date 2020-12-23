/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "ptsetts.hpp"

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

modegen::ic::gen_settings ptsetts::part_setts(std::string_view p) const
{
	modegen::ic::gen_settings ret;
	ret.map_tmpl = setts.get<std::pmr::string>("part."s+std::string(p)+".file"s);
	ret.tmpl_file = setts.get<std::pmr::string>("part."s+std::string(p)+".tmpl"s);
	return ret;
}
