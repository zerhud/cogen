/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "cmake.hpp"

#include "part_manager.hpp"
#include "output_configurator.hpp"

#include "exceptions.hpp"

namespace mpg = modegen::pg;
namespace mpo = modegen::pg::outputs;
using namespace std::literals;

	//FS::path out_file_;
	//nlohmann::json data_;
	//std::vector<part_algos_ptr> imngs_;
	//const options::part_view opts_;

mpo::cmake::cmake(FS::path o, std::vector<std::any> data)
    : out_file_(std::move(o))
    , inputs_(std::move(data))
{
}

mpg::output_lang mpo::cmake::lang() const
{
	return output_lang::cmake;
}

void modegen::pg::outputs::cmake::setts(const modegen::pg::options::part_view& s)
{
	opts_ = s;
}

nlohmann::json mpo::cmake::data(const part_manager& pman) const
{
	using boost::property_tree::ptree;
	if(!opts_) throw errors::error("cmake generator cannot work without options"s);

	nlohmann::json data;

	ptree part = opts_->get_subset(options::subsetts::part_data);
	data["project"] = part.get<std::string>("project");
	data["version"] = part.get("version", "0.0.0.0"s);

	// add_library..
	auto libs = part.get_child_optional("libraries");
	for(auto l:*libs) {
		std::vector<std::string> files;
		std::vector<std::string> target_link_libraries;
		std::map<std::string,std::vector<std::string>> libs;
		for(auto& f:l.second) {
			auto val = f.second.get_value<std::string>();
			if(f.first=="file"sv) files.emplace_back(val);
			if(f.first=="dep"sv) target_link_libraries.emplace_back(val);
			if(f.first=="part"sv) {
				auto mlibs = pman.require(val)->map_from(l.first);
				for(auto& [n,l]:mlibs) for(auto& i:l) libs[n].emplace_back(i);
			}
		}

		const bool was_mapped = !libs.empty();
		if(was_mapped) {
			for(auto& [lname,pdeps]:libs) {
				data["libraries"][lname]["files"] = pdeps;
				for(auto& file:files) data["libraries"][lname]["files"].emplace_back(file);
				data["libraries"][lname]["target_link_libraries"] = target_link_libraries;
			}
		}
		else {
			data["libraries"][l.first]["files"] = files;
			data["libraries"][l.first]["target_link_libraries"] = target_link_libraries;
		}
	}

	// find_package..
	auto deps = part.get_child_optional("deps");
	if(deps) for(auto& d:*deps) {
		nlohmann::json ddata;
		ddata["pack"] = d.first;
		data["deps"].push_back(ddata);
	}

	return data;
}

FS::path mpo::cmake::file() const
{
	return out_file_;
}

