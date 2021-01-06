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
	gen_utils::imports_manager imports = make_imports(cur_part, compiled);
	for(auto& [n,d]:compiled)
		outside->generate(
		            cur_part.cfg_part.tmpl_file,
		            make_json(cur_part, d, imports),
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

gen_utils::imports_manager single_gen_part::make_imports(
        const mdg::ic::gen_context& setts,
        const compiled_output& result) const
{
	gen_utils::imports_manager imports;
	for(auto& [n,d]:result) imports.add(n, d);
	for(auto& link:setts.cfg_part.links)
		for(auto& [n,d]:setts.generated.at(link))
			imports.add(n,d);
	imports.build();
	return imports;
}

boost::json::value single_gen_part::make_json(
        const gen_context& setts,
        const input& data,
        const gen_utils::imports_manager& imports) const
{
	gen_utils::compilation_context ctx{.cfg = setts.cfg_part.compilation};
	boost::json::object result;
	boost::json::array& data_ar = result["data"].emplace_array();
	for(auto& it:data.all()) data_ar.emplace_back(it->to_json(ctx));
	add_includes_to_result(result, data, imports);
	return result;
}
void single_gen_part::add_includes_to_result(
        boost::json::object& result,
        const gen_utils::input& data,
        const gen_utils::imports_manager& imports) const
{
	boost::json::object& incs = result["includes"].emplace_object();
	boost::json::array& mincs = incs["matched"].emplace_array();
	for(auto& l:imports.self_matched(data))
		mincs.emplace_back(l);
	boost::json::object& rincs = incs["required"].emplace_object();
	for(auto& r:imports.required_for(data))
		rincs[r.cond].emplace_array().emplace_back(r.file);
}
