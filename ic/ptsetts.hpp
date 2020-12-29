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
	void conf_links(const std::string& path, modegen::ic::gen_context& ctx) const ;
public:
	ptsetts(boost::property_tree::ptree s);

	std::pmr::vector<std::pmr::string> parts() const ;
	void part_setts(std::string_view p, modegen::ic::gen_context& ctx) const ;
};

} // namespace mdg::ic
