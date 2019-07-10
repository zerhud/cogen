/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "noinput.hpp"
#include "pg/provider.hpp"

namespace mpg = modegen::pg;

mpg::parts::noinput::noinput(mpg::provider_ptr p, mpg::options::part_view s)
    : prov(std::move(p))
    , setts(std::move(s))
{
}

mpg::output_lang mpg::parts::noinput::lang() const
{
	return from_string(setts.get<std::string>(options::part_option::output_lang));
}

std::string_view mpg::parts::noinput::name() const
{
	return setts.part();
}

std::vector<mpg::output_descriptor_ptr> mpg::parts::noinput::outputs() const
{
	return {out_};
}

void mpg::parts::noinput::build_outputs(const mpg::part_manager& pman, const mpg::provider& prov)
{
	assert(lang()==output_lang::cmake);
	out_ = prov.create_output(lang(),setts.output_tmpl());
}
