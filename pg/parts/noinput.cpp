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

mpg::parts::noinput::noinput(mpg::options::part_view s)
    : setts(std::move(s))
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

void mpg::parts::noinput::build_outputs(const mpg::part_manager& pman, mpg::provider_const_ptr prov)
{
	assert(lang()==output_lang::cmake);
	prov_ = prov;

	out_ = prov_->create_output(lang(),setts.output_tmpl());
}

std::vector<std::string> mpg::parts::noinput::map_to_outputs(const std::string& tmpl) const
{
	return {};
}

