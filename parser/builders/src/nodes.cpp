/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "nodes.hpp"
#include "utils/map_from.hpp"

using namespace std::literals;

std::string_view builders::base_node::name() const
{
	return ""sv;
}

std::pmr::vector<gen_utils::name_t> builders::base_node::required_links() const
{
	return {};
}

std::string_view builders::base_node::link_condition() const
{
	return name();
}

std::optional<gen_utils::import_file> builders::base_node::imports_modification() const
{
	return std::nullopt;
}

std::optional<uint64_t> builders::base_node::version() const
{
	return 0;
}

std::optional<gen_utils::variable> builders::base_node::node_var() const
{
	return std::nullopt;
}


std::string_view builders::root_node::name() const
{
	return "builders"sv;
}

boost::json::value builders::root_node::to_json(
        const gen_utils::tree& con,
        const gen_utils::compilation_context& ctx) const
{
	boost::json::object ret;
	for(auto& child:con.children(*this)) {
		assert(dynamic_cast<const base_node*>(child.get()));
		auto cj = static_cast<const base_node&>(*child.get())
		        .to_json(con,ctx).as_object();
		for(auto& jnode:cj) ret[jnode.key()] = jnode.value();
	}
	return ret;
}

builders::project::project(std::string n, std::string v)
	: pname(std::move(n))
	, ver(std::move(v))
{
}

boost::json::value builders::project::to_json(
        const gen_utils::tree& con,
        const gen_utils::compilation_context& ctx) const
{
	boost::json::object libs;
	for(auto& child:con.children(*this)) {
		auto lib = static_cast<const base_node&>(*child)
		        .to_json(con,ctx).as_object();
		for(auto& [name,val]:lib) libs[name]=std::move(val);
	}
	boost::json::object ret;
	ret["project"] = pname;
	ret["libraries"] = libs;
	ret["version"] = ver;
	return ret;
}

builders::library::library(
        std::string n,
        boost::property_tree::ptree lsetts,
        const mdg::ic::gen_context& ctx)
    : lib(std::move(n))
    , setts(std::move(lsetts))
{
	using gen_utils::map_from;
	for(auto& [name, opts]:lsetts) {
		auto strval = opts.get_value<std::pmr::string>();
		if(name == "part") {
			for(auto& [file, data]:ctx.generated.at(strval)) {
			//	map_from()(data, name, fuck);
				files.emplace_back(file);
			}
		} else if(name == "dep") deps.emplace_back(strval);
		else if(name == "link_lib") libs.emplace_back(strval);
	}
}

boost::json::value builders::library::to_json(
        const gen_utils::tree& con,
        const gen_utils::compilation_context& ctx) const
{
	assert(ctx.links);
	assert(ctx.all_input);
	boost::json::object ret;
	auto mr = ctx.links->map_from(lib, *ctx.all_input);
	for(auto& [n,cnt]:mr) {
		boost::json::object& cur_lib = ret[n].emplace_object();
		cur_lib["deps"] = make_json_deps();
		cur_lib["link_libs"] = make_json_libs();
		cur_lib["files"] = make_json_files(cnt);
	}
	return ret;
}

boost::json::value builders::library::make_json_deps() const
{
	boost::json::array ret;
	for(auto& d:deps) ret.emplace_back(d);
	return ret;
}

boost::json::value builders::library::make_json_libs() const
{
	boost::json::array ret;
	for(auto& l:libs) ret.emplace_back(l);
	return ret;
}

boost::json::value builders::library::make_json_files(
        const std::pmr::vector<std::pmr::string>& list) const
{
	boost::json::array ret;
	for(auto& c:list) ret.emplace_back(c);
	return ret;
}
