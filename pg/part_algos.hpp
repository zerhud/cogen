/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include <boost/property_tree/ptree.hpp>
#include "declarations.hpp"

namespace modegen::pg {

class part_algos {
public:
	virtual ~part_algos() noexcept =default ;
	virtual void set_filter(const options::part_view& pinfo) =0 ;
	virtual std::vector<std::string> map(const std::string& tmpl) const =0 ;
};

} // namespace modegen::pg

