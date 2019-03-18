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

std::vector<mg::file_data::output_info> mg::cmake::jsoned_data(const std::vector<parser::loader_ptr>& data_loaders, options::view opts) const
{
	(void) data_loaders;

	nlohmann::json data;
	auto part = opts.get_subset(options::subsetts::part_data);
	data["project"] = part.get<std::string>("project");
	data["version"] = part.get("version", u8"0.0.0.0"s);

	auto libs = part.get_child_optional("libraries");
	for(auto l:*libs) {
		std::vector<std::string> files;
		for(auto& f:l.second) {
			auto val = f.second.get_value<std::string>();
			if(f.first=="file"sv) files.emplace_back(val);
			if(f.first=="part"sv) files.emplace_back(opts.get<std::string>(options::part_option::output, val));
		}


		data["libraries"][l.first]["files"] = files;
	}

	output_info ret;
	ret.data = std::move(data);
	ret.out_file = opts.get<std::string>(options::part_option::output);
	return {ret};
}
