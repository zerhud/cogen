/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include <memory_resource>
#include <boost/property_tree/ptree.hpp>

#include "single_gen_part.hpp"

namespace cogen::ic {

class ptsetts final {
	boost::property_tree::ptree setts;
	void conf_links(const std::string& path, gen_config& cfg) const ;
	void config_compilation(
	        const std::string& path, gen_utils::compilation_config& cfg) const ;
public:
	ptsetts(boost::property_tree::ptree s);

	boost::property_tree::ptree part_src(std::string_view name) const ;

	std::pmr::vector<std::pmr::string> parts() const ;
	gen_config part_setts(std::string_view p) const ;
	gen_utils::tree generic_ast(std::string_view p) const ;
};

} // namespace cogen::ic
