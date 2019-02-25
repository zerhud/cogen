/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "cmake.hpp"
#include <boost/property_tree/json_parser.hpp>

using namespace std::literals;
namespace mg = modegen::generation;
using boost::property_tree::ptree;

cppjson::value mg::cmake::jsoned_data(const std::vector<parser::loader_ptr>& data_loaders, options_view opts) const
{
	(void) data_loaders;

	cppjson::value data;
	data["project"] = opts.part_data().get<std::string>("project");
	data["version"] = opts.part_data().get("version", u8"0.0.0.0"s);

	ptree part_data = opts.part_data();
	auto libs = part_data.get_child_optional("libraries");
	for(auto l:*libs) {
		std::vector<std::string> files;
		for(auto& f:l.second) {
			auto val = f.second.get_value<std::string>();
			if(f.first=="file"sv) files.emplace_back(val);
			if(f.first=="part"sv) files.emplace_back(opts.part_data(val).get<std::string>("output"));
		}


		data["libraries"][l.first]["files"] = files;
	}

	return data;
}
