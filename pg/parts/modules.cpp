/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "modules.hpp"

#include "parser/data_tree/loader.hpp"
#include "parser/interface/loader.hpp"
#include "parser/interface/helpers.hpp"

#include "pg/provider.hpp"
#include "pg/exceptions.hpp"

namespace mpg = modegen::pg;
namespace mpp = modegen::pg::parts;
namespace mpi = modegen::parser::interface;

using namespace std::literals;

std::tuple<mpp::module_part::fgmode,std::string> mpp::module_part::outinfo() const
{
	auto tmpl = setts.output_tmpl();
	auto mode = setts.output_mode();

	if(mode=="file_bymod"s) return std::make_tuple(fgmode::bymod, tmpl);
	return std::make_tuple(fgmode::single, tmpl);
}

mpp::module_part::module_part(provider_ptr p, options::part_view s)
    : prov(std::move(p))
    , setts(std::move(s))
{
	if(!prov) throw errors::error("cannot create module_part without provider");
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

void mpp::module_part::build_outputs(const mpg::part_manager& pman, const mpg::provider& prov)
{
	// file_single filter..
	// file_bymod (name with tmpl) filter...
	auto inputs = prov.input();
	std::shared_ptr<mpi::loader> mods_ldr;
	for(auto& i:inputs) if(mods_ldr = std::dynamic_pointer_cast<mpi::loader>(i); mods_ldr) break;
	if(!mods_ldr) throw errors::error("part "s + std::string(name()) + " requires module input"s);
	auto mods = mods_ldr->result();

	auto [mode, ftmpl] = outinfo();
	if(mode==fgmode::single) {
		outs_.emplace_back(prov.create_output(lang(), ftmpl));
	}
	else if(mode==fgmode::bymod) {
		for(auto& mod:mods) {
			std::string tmpl = ftmpl;
			auto mpos = tmpl.find("$mod");
			if(mpos!=std::string::npos) tmpl.replace(mpos,4,mod.name);
			auto ver = mpi::get_version(mod);
			auto vpos = tmpl.find("$va");
			if(vpos!=std::string::npos) tmpl.replace(vpos,3,std::to_string(ver.major_v));
			vpos = tmpl.find("$vi");
			if(vpos!=std::string::npos) tmpl.replace(vpos,3,std::to_string(ver.minor_v));
			outs_.emplace_back(prov.create_output(lang(), tmpl));
		}
	}
	else if(mode==fgmode::byent) {
		throw errors::notready("byent output mode");
	}
}
