/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "info_part.hpp"

#include "provider.hpp"
#include "output_descriptor.hpp"
#include "part_algos.hpp"
#include "exceptions.hpp"

namespace mpg = modegen::pg;
namespace mpo = modegen::pg::options;

using namespace std::literals;

void mpg::info_part::create_algos(const provider& prov)
{
	algos_.clear();
	algos_.emplace_back(prov.create_algos(input_lang::mdl));
	algos_.emplace_back(prov.create_algos(input_lang::data));
}

std::tuple<mpg::info_part::fgmode,std::string> mpg::info_part::outinfo() const
{
	auto tmpl = setts.output_tmpl();
	auto mode = setts.output_mode();

	if(mode=="file_map"s) return std::make_tuple(fgmode::map, tmpl);
	return std::make_tuple(fgmode::single, tmpl);
}

mpg::info_part::info_part(options::part_view s)
    : setts(std::move(s))
{
}

FS::path mpg::info_part::tmpl_file() const
{
	assert(prov_);
	auto file = setts.get<std::string>(mpo::part_option::input);
	return prov_->resolve_file(file, setts.container()->opts_dir(), lang());
}

mpg::output_lang mpg::info_part::lang() const
{
	return from_string(setts.get<std::string>(options::part_option::output_lang));
}

std::string_view mpg::info_part::name() const
{
	return setts.part();
}

std::vector<mpg::output_descriptor_ptr> mpg::info_part::outputs() const
{
	if(outs_.empty()) throw errors::error("no outs: call build_outputs first");
	return outs_;
}

void mpg::info_part::build_outputs(mpg::provider_const_ptr prov)
{
	prov_ = prov;
	if(!prov_) throw errors::error("cannot build outputs without provider");

	create_algos(*prov_);
	assert(!algos_.empty());

	for(auto& alg:algos_) {
		assert(alg);
		alg->set_filter(setts);
	}

	auto [mode, ftmpl] = outinfo();

	configuration_algos::mapped_data mapped;
	mapped[ftmpl] = {};

	for(auto& alg:algos_) mapped = alg->map_to(std::move(mapped));
	for(auto mi:mapped) {
		auto out = outs_.emplace_back(prov_->create_output(lang(), mi.first, mi.second));
		out->setts(opts());
	}
}

std::vector<mpg::part_algos_ptr> mpg::info_part::input_managers() const
{
	return algos_;
}

std::map<std::string,std::vector<std::string>> mpg::info_part::map_from(const std::string& tmpl) const
{
	std::map<std::string,std::vector<std::string>> ret;
	for(auto& alg:algos_) {
		auto map = alg->map_from(tmpl);
		for(auto& [k,v]:map) for(auto& i:v) ret[k].emplace_back(i);
	}
	return ret;
}

mpg::options::part_view mpg::info_part::opts() const
{
	return setts;
}

