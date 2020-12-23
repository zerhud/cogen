/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "generation_part.hpp"
#include "common_utils/input/map_to.hpp"

using modegen::ic::input;
using modegen::ic::single_gen_part;

single_gen_part::single_gen_part(const provider* p)
	: outside(std::move(p))
{
	if(!outside)
		throw std::runtime_error("cannot create single generation part without provider");
}

void single_gen_part::operator()(gen_settings cur_part, input alli) const
{
	assert(outside);
	if(!cur_part.gen_cfg) throw std::runtime_error("no gen configuration in settings");
	for(auto& [n,d]:compile(cur_part, alli))
		outside->generate(cur_part.tmpl_file, make_json(cur_part, d), n);
}

std::pmr::map<std::pmr::string, input> single_gen_part::compile(
    const gen_settings& setts, const input& data) const
{
	gen_utils::map_to mapper;
	std::pmr::map<std::pmr::string, input> ret;
	for(auto& it:data.all()) {
		auto compiled = mapper(std::pmr::string(setts.map_tmpl), *it);
		for(auto& [k,v] : compiled) ret[k].add(std::move(v));
	}
	return ret;
}

boost::json::value single_gen_part::make_json(
        const gen_settings& setts, const input& data) const
{
	boost::json::array data_ar;
	for(auto& it:data.all())
		data_ar.emplace_back(it->to_json(*setts.gen_cfg));
	return data_ar;
}
