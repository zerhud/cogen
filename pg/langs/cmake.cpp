/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "cmake.hpp"

#include "part_manager.hpp"
#include "part_descriptor.hpp"

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

	auto libs = part.get_child_optional("libraries");
	for(auto l:*libs) {
		std::vector<std::string> deps;
		std::vector<std::string> files;
		for(auto& f:l.second) {
			auto val = f.second.get_value<std::string>();
			if(f.first=="file"sv) files.emplace_back(val);
			if(f.first=="part"sv) files_from_part(files, pman.require(val).get());
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

FS::path mpo::cmake::file() const
{
	return out_file_;
}

void mpo::cmake::files_from_part(std::vector<std::string>& files, part_descriptor* part) const
{
	assert( part );
	auto olist = part->outputs();
	for(auto& o:olist) files.emplace_back(o->file());
}

