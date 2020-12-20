/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "generation_part.hpp"
#include "common_utils/input/map_to.hpp"

using modegen::ic::single_gen_part;

single_gen_part::single_gen_part(std::shared_ptr<provider> p)
	: outside(std::move(p))
{
}

void single_gen_part::operator()(gen_settings cur_part, input alli) const
{
	assert(outside);
	gen_utils::map_to mapper;
	std::map<std::pmr::string, input> data;
	for(auto& it:alli.all()) {
		auto compiled = mapper(std::pmr::string(cur_part.map_tmpl), *it);
		for(auto& [k,v] : compiled) data[k].add(std::move(v));
	}

	for(auto& [n,d]:data) {
		boost::json::value data;
		//boost::json::array& data_ar = data.as_array();
		//for(auto& it:d.all()) {
		//}
		outside->generate(cur_part.tmpl_file, data, n);
	}
}

