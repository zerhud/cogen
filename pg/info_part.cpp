/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "info_part.hpp"

#include "parser/data_tree/loader.hpp"
#include "parser/interface/loader.hpp"
#include "parser/interface/helpers.hpp"

#include "provider.hpp"
#include "output_descriptor.hpp"
#include "exceptions.hpp"

namespace mpg = modegen::pg;
namespace mpp = modegen::pg::parts;
namespace mpi = modegen::parser::interface;

using namespace std::literals;

std::tuple<mpp::module_part::fgmode,std::string> mpp::module_part::outinfo() const
{
	auto tmpl = setts.output_tmpl();
	auto mode = setts.output_mode();

	if(mode=="file_bymod"s) return std::make_tuple(fgmode::bymod, tmpl);
	if(mode=="file_byent"s) return std::make_tuple(fgmode::byent, tmpl);
	return std::make_tuple(fgmode::single, tmpl);
}

mpp::module_part::module_part(options::part_view s)
    : setts(std::move(s))
{
	auto lng = lang();
	assert( lng==output_lang::cpp || lng==output_lang::python || lng==output_lang::javascript );
}

mpg::output_lang mpp::module_part::lang() const
{
	return from_string(setts.get<std::string>(options::part_option::output_lang));
}

std::string_view mpp::module_part::name() const
{
	return setts.part();
}

std::vector<mpg::output_descriptor_ptr> mpp::module_part::outputs() const
{
	if(outs_.empty()) throw errors::error("no outs: call build_outputs first");
	return outs_;
}

void mpp::module_part::build_outputs(const mpg::part_manager& pman, mpg::provider_const_ptr prov)
{
	prov_ = prov;
	if(!prov_) throw errors::error("cannot build outputs without provider");

	// file_single filter..
	// file_bymod (name with tmpl) filter...

	auto [mode, ftmpl] = outinfo();
	if(mode==fgmode::single) {
		auto out = outs_.emplace_back(prov_->create_output(lang(), ftmpl));
		out->override_setts(setts.get_subset(options::subsetts::part_data));
	}
	else if(mode==fgmode::bymod) {
		auto osetts = setts.get_subset(options::subsetts::part_data);
		auto tmpls = map_to_outputs(ftmpl);
		for(auto& tmpl:tmpls) {
			auto out = outs_.emplace_back(prov_->create_output(lang(), tmpl));
			out->override_setts(osetts);
		}
	}
	else if(mode==fgmode::byent) {
		throw errors::notready("byent output mode");
	}
}

std::vector<std::string> mpp::module_part::map_to_outputs(const std::string& tmpl) const
{
	if(!prov_) throw errors::error("cannot map to outputs without provider");
	std::vector<std::string> ret;

	auto inputs = prov_->input();
	std::shared_ptr<mpi::loader> mods_ldr;
	for(auto& i:inputs) if(mods_ldr = std::dynamic_pointer_cast<mpi::loader>(i); mods_ldr) break;
	if(!mods_ldr) throw errors::error("part "s + std::string(name()) + " requires module input"s);
	auto mods = mods_ldr->result();

	//TODO: add mod's filter here

	for(auto& mod:mods) {
		std::string& cur = ret.emplace_back(tmpl);
		auto ver = mpi::get_version(mod);
		replace(cur, "$mod"s, mod.name);
		replace(cur, "$va"s, std::to_string(ver.major_v));
		replace(cur, "$vi"s, std::to_string(ver.minor_v));
	}

	return ret;
}

bool mpp::module_part::replace(std::string& tmpl, const std::string& var_name, const std::string& value) const
{
	auto pos = tmpl.find(var_name);
	bool found = pos!=std::string::npos;
	if(found) tmpl.replace(pos, var_name.size(), value);
	return found;
}

