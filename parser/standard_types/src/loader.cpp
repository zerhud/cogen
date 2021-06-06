/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "loader.hpp"

#include <fstream>
#include <boost/property_tree/info_parser.hpp>

#include "nodes.hpp"
#include "manager.hpp"

using std_types::loader;

gen_utils::tree loader::load_types(std::filesystem::path file) const
{
	std::fstream f(file, std::ios_base::in);
	return load_types(f);
}

gen_utils::tree loader::load_types(std::istream& src) const
{
	gen_utils::tree ret(std::make_shared<root_node>(), std::make_shared<manager>());

	boost::property_tree::ptree parsed;
	boost::property_tree::read_info(src, parsed);
	for(auto& type:parsed) {
		for(auto& target:type.second) {
			auto ti = std::make_shared<type_node>(
			            type.first,
			            target.first,
			            target.second.get<std::string>("replace"),
			            target.second.get<std::pmr::string>("file"),
			            target.second.get<bool>("sys"));
			ret.add(ret.root(), std::move(ti));
		}
	}

	return ret;
}
