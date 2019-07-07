/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "module_part.hpp"
#include "parser/interface/loader.hpp"
#include "parser/data_tree/loader.hpp"

#include "exceptions.hpp"

namespace mpg = modegen::pg;

mpg::module_part::module_part(provider_ptr p, options::part_view s)
    : prov(std::move(p))
    , setts(std::move(s))
{
	if(!prov) throw errors::error("cannot create module_part without provider");
}

modegen::pg::output_lang modegen::pg::module_part::lang() const
{
	return from_string(setts.get<std::string>(options::part_option::output_lang));
}

std::string_view modegen::pg::module_part::name() const
{
	return setts.part();
}

std::vector<modegen::pg::output_descriptor_ptr> modegen::pg::module_part::outputs() const
{
	return outs_;
}

void modegen::pg::module_part::build_outputs(const modegen::pg::part_manager& pman, const modegen::pg::provider& prov)
{
	;
}
