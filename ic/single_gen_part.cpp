/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "single_gen_part.hpp"
#include "utils/map_to.hpp"
#include "utils/split_by_ver.hpp"

using gen_utils::input;
using cogen::ic::single_gen_part;
using cogen::ic::compiled_output;

single_gen_part::single_gen_part(const provider* p)
	: outside(std::move(p))
{
	if(!outside)
		throw std::runtime_error(
				"cannot create single generation "
				"part without provider");
}

compiled_output single_gen_part::operator()(
        const gen_context& cur_part, input alli) const
{
	assert(outside);
	input splitted = split_by_vers(cur_part.cfg_part, std::move(alli));
	auto compiled = gen_utils::map_to()(cur_part.cfg_part.map_tmpl, splitted);
	gen_utils::imports_manager imports = make_imports(cur_part, compiled);
	for(auto& [n,d]:compiled) {
		d.conf() = cur_part.cfg_part.compilation;
		auto jdata = add_includes_to_result(
		            make_json_result(cur_part, d, imports).as_object(),
		            cur_part.cfg_part.include_own_part
		                ? imports.required_includes_with_own(d)
		                : imports.required_includes(d),
		            imports.mapped_includes(cur_part.cfg_part.map_tmpl,d)[n]);
		outside->generate(
		            cur_part.cfg_part.tmpl_file,
		            jdata, n);
	}
	return compiled;
}

gen_utils::input single_gen_part::split_by_vers(
    const gen_config& setts, gen_utils::input data) const
{
	if(!setts.split_by_version) return data;
	return data.modify([](const gen_utils::tree& t){
		auto splited = gen_utils::split_by_ver{}(t);
		assert(!splited.empty());
		auto first = splited.begin();
		for(auto pos = first+1;pos!=splited.end();++pos)
			first->merge(*pos);
		return std::pmr::vector<gen_utils::tree>{*first};
	});
}

gen_utils::imports_manager single_gen_part::make_imports(
        const cogen::ic::gen_context& setts,
        const compiled_output& result) const
{
	gen_utils::imports_manager imports;
	for(auto& [n,d]:result) imports.add("", n, d);
	for(auto& link:setts.cfg_part.links)
		for(auto& [n,d]:setts.generated.at(link))
			imports.add(link, n,d);
	if(setts.cfg_part.links.empty()) {
		for(auto& [link, ld]:setts.generated)
			for(auto& [n,d]:ld) imports.add(link, n,d);
	}
	return imports;
}

boost::json::value single_gen_part::make_json_result(
        const gen_context& setts,
        const input& data,
        const gen_utils::imports_manager& imports) const
{
	gen_utils::compilation_context ctx{
		.cfg = setts.cfg_part.compilation,
		.links = &imports,
		.all_input = &data };
	boost::json::object result;
	boost::json::object& data_ar = result["data"].emplace_object();
	for(auto& it:data.all()) data_ar[it->data_id()] = it->to_json(ctx);
	return result;
}

boost::json::object single_gen_part::add_includes_to_result(
        boost::json::object result,
        gen_utils::imports_manager::incs_map_t required,
        std::pmr::vector<gen_utils::import_file> mapped) const
{
	boost::json::object& jincs = result["includes"].emplace_object();
	for(auto& [cond, files]:required) {
		boost::json::array& far = jincs[cond].emplace_array();
		for(auto& f:files) far.emplace_back(to_json(f));
	}
	if(!mapped.empty()) {
		auto& jar = jincs["self"].emplace_array();
		for(auto& f:mapped) jar.emplace_back(to_json(f));
	}
	return result;
}

boost::json::object single_gen_part::to_json(const gen_utils::import_file& f) const
{
	boost::json::object ret;
	ret["file"] = f.name;
	ret["sys"] = f.sys;
	return ret;
}
