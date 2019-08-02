/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "data.hpp"

namespace ma = modegen::pg::palgos ;
namespace mp = modegen::parser;

ma::data_algos::data_algos(const std::vector<mp::loader_ptr>& ldrs)
{
}


std::vector<std::string> ma::data_algos::map_to(const std::string& tmpl)
{
	(void)tmpl;
	return {};
}

void ma::data_algos::set_filter(const options::part_view& pinfo)
{
	(void)pinfo;
}

std::vector<std::string> ma::data_algos::map(const std::string& tmpl) const
{
	(void)tmpl;
	return {};
}

