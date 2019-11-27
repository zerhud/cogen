/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "../configuration_algos.hpp"
#include "parser/loader.hpp"

namespace modegen::pg::palgos {

class data_algos : public configuration_algos {
public:
	data_algos(const std::vector<modegen::parser::loader_ptr>& ldrs);
	void set_filter(const options::part_view& pinfo) override ;
	std::vector<std::string> map(const std::string& tmpl) const override ;
	mapped_data map_to(mapped_data md) override ;
	std::map<std::string,std::vector<std::string>> map_from(const std::string& tmpl) override ;
};

} // namespace modegen::pg::palgos


