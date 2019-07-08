/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "modules.hpp"

#include "parser/interface/loader.hpp"
#include "parser/data_tree/loader.hpp"

#include "pg/provider.hpp"
#include "pg/exceptions.hpp"

namespace mpg = modegen::pg;
namespace mpp = modegen::pg::parts;

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
	auto mods = prov.input();
}
