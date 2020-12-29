/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "single_gen_part.hpp"
#include "common_utils/input/map_to.hpp"

using modegen::ic::input;
using modegen::ic::single_gen_part;
using modegen::ic::compiled_output;

single_gen_part::single_gen_part(const provider* p)
	: outside(std::move(p))
{
	if(!outside)
		throw std::runtime_error(
				"cannot create single generation "
				"part without provider");
}

compiled_output single_gen_part::operator()(const gen_context& cur_part, input alli) const
{
	assert(outside);
	if(!cur_part.gen_cfg)
		throw std::runtime_error("no gen configuration in settings");
	auto compiled = compile(cur_part, alli);
	for(auto& [n,d]:compiled)
		outside->generate(
				cur_part.tmpl_file,
				make_json(cur_part, d),
				n);
	return compiled;
}

compiled_output single_gen_part::compile(
        const gen_context& setts, const input& data) const
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
        const gen_context& setts, const input& data) const
{
	boost::json::array data_ar;
	for(auto& it:data.all()) {
		auto& dobj = data_ar.emplace_back(make_json(setts, *it)).as_object();
		auto& incs = dobj["includes"].emplace_array();
		for(auto& link:setts.links) {
			for(auto& l:matched_includes(setts, link, data))
				incs.emplace_back(l);
		}
	}
	return data_ar;
}

std::pmr::vector<std::pmr::string> single_gen_part::matched_includes(
    const gen_context& setts, const std::pmr::string& link, const input& data) const
{
	std::pmr::vector<std::pmr::string> ret;
	const compiled_output& ldata = setts.generated.at(link);
	for(auto& [dn, dt]:ldata) {
		auto rc = dt.contains(data);
		if(rc==gen_utils::tree_compare_result::total)
			ret.emplace_back(dn);
		else if(rc==gen_utils::tree_compare_result::partial)
			ret.emplace_back(dn);
	}
	return ret;
}

boost::json::value single_gen_part::make_json(
        const gen_context& setts,
        const gen_utils::tree& data) const
{
	auto ret = data.to_json(*setts.gen_cfg).as_object();
	//auto& incs = ret["includes"].emplace_array();
	//for(auto& link:setts.links) {
	    //const compiled_output& ldata = setts.generated.at(link);
	    //for(auto& [dn, dt]:ldata) {
	        //incs.emplace_back().as_object();
	    //}
	//}
	return ret;
}
