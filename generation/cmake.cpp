/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "cmake.hpp"
#include <boost/property_tree/json_parser.hpp>

#include "output_info.hpp"
#include "part_descriptor.hpp"

using namespace std::literals;
namespace mg = modegen::generation;
using boost::property_tree::ptree;

nlohmann::json mg::cmake::jsoned_data(const output_info& outputs) const
{
	const mg::options::view& opts = outputs.current_part()->opts();

	nlohmann::json data;
	ptree part = opts.get_subset(options::subsetts::part_data);
	data["project"] = part.get<std::string>("project");
	data["version"] = part.get("version", u8"0.0.0.0"s);

	auto libs = part.get_child_optional("libraries");
	for(auto l:*libs) {
		std::vector<std::string> deps;
		std::vector<std::string> files;
		for(auto& f:l.second) {
			auto val = f.second.get_value<std::string>();
			if(f.first=="file"sv) files.emplace_back(val);
			//if(f.first=="part"sv) files.emplace_back(opts.get<std::string>(options::part_option::output, val));
			if(f.first=="part"sv) files_from_part(files, outputs.require(val));
			if(f.first=="dep"sv) deps.emplace_back(val);
		}


		data["libraries"][l.first]["files"] = files;
		data["libraries"][l.first]["deps"] = deps;
	}

	auto deps = part.get_child_optional("deps");
	if(deps) for(auto& d:*deps) {
		nlohmann::json ddata;
		ddata["pack"] = d.first;
		data["deps"].push_back(ddata);
	}

	return data;
}

void mg::cmake::files_from_part(std::vector<std::string>& files, part_descriptor* part) const
{
	assert( part );
	do {
		files.emplace_back(part->file_name());
	} while(part->next());
}

