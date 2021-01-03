/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "single_gen_part.hpp"
#include "common_utils/input/map_to.hpp"

using gen_utils::input;
using mdg::ic::single_gen_part;
using mdg::ic::compiled_output;

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
	auto compiled = compile(cur_part, alli);
	for(auto& [n,d]:compiled)
		outside->generate(
		            cur_part.cfg_part.tmpl_file,
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
		auto compiled = mapper(std::pmr::string(setts.cfg_part.map_tmpl), *it);
		for(auto& [k,v] : compiled) ret[k].add(std::move(v));
	}
	return ret;
}

boost::json::value single_gen_part::make_json(
        const gen_context& setts, const input& data) const
{
	gen_utils::compilation_context ctx{.cfg = setts.cfg_part.compilation};
	boost::json::array data_ar;
	for(auto& it:data.all()) {
		auto& dobj = data_ar.emplace_back(it->to_json(ctx)).as_object();
		auto& incs = dobj["includes"].emplace_array();
		for(auto& link:setts.cfg_part.links) {
			for(auto& l:matched_includes(setts, link, data))
				incs.emplace_back(l);
		}
	}
	return data_ar;
}

std::pmr::vector<std::pmr::string> single_gen_part::matched_includes(
        const gen_context& setts,
        const std::pmr::string& link,
        const input& data) const
{
	using gen_utils::tree_compare_result;
	std::pmr::vector<std::pmr::string> ret;
	const compiled_output& ldata = setts.generated.at(link);
	for(auto& [dn, dt]:ldata) {
		auto rc = dt.match_with(data);
		if(rc==tree_compare_result::total)
			ret.emplace_back(dn);
		else if(rc==tree_compare_result::partial)
			ret.emplace_back(dn);
	}
	return ret;
}
