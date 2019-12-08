/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "ix3.hpp"

#include "options.hpp"

#include "parser/interface/ix3_loader.hpp"
#include "ix3/utils/filter.hpp"
#include "ix3/utils/remove_empty.hpp"
#include "ix3/utils/split_by_version.hpp"
#include "ix3/utils/naming.hpp"

using namespace std::literals;
using namespace modegen::pg::conf_algos;

ix3_conf::ix3_conf(const std::vector<modegen::parser::loader_ptr>& ldrs)
{
	for(auto& ldr:ldrs) {
		auto ix3ldr = dynamic_cast<parser::interface::ix3_loader*>(ldr.get());
		if(ix3ldr) mods_ = ix3ldr->ix3_result();
	}
}

void ix3_conf::set_filter(const options::part_view& pinfo)
{
	using namespace ix3::utils;
	using options::part_option;

	filter::request req;
	std::string versioning = pinfo.get_opt<std::string>(options::template_option::versioning).value_or("merge"s);
	gen_utils::name_conversion nc;
	gen_utils::from_string(pinfo.get<std::string>(part_option::naming), nc);
	mods_
		| split_by_version(versioning != "split"sv)
		| filter(req)
		| naming(nc)
		| remove_empty{}
		;
}

std::vector<std::string> ix3_conf::map(const std::string& tmpl) const
{
	return {};
}

ix3_conf::mapped_data ix3_conf::map_to(mapped_data md)
{
	return {};
}

std::map<std::string,std::vector<std::string>> ix3_conf::map_from(const std::string& tmpl)
{
	return {};
}
