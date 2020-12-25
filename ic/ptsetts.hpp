/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include <memory_resource>
#include <boost/property_tree/ptree.hpp>

#include "single_gen_part.hpp"

namespace mdg::ic {

class ptsetts final {
	boost::property_tree::ptree setts;
public:
	ptsetts(boost::property_tree::ptree s);

	std::pmr::vector<std::pmr::string> parts() const ;
	modegen::ic::gen_settings part_setts(std::string_view p) const ;
};

} // namespace mdg::ic
