/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "ptsetts.hpp"
#include <boost/property_tree/json_parser.hpp>

using mdg::ic::ptsetts;
using namespace std::literals;

ptsetts::ptsetts(boost::property_tree::ptree s)
	: setts(std::move(s))
{
}

boost::property_tree::ptree ptsetts::part_src(std::string_view name) const
{
	return setts.get_child("part."s + std::string(name));
}

std::pmr::vector<std::pmr::string> ptsetts::parts() const
{
	std::pmr::vector<std::pmr::string> ret;
	for(auto& p:setts.get_child("part"))
		ret.emplace_back(p.first);
	return ret;
}

mdg::ic::gen_config ptsetts::part_setts(std::string_view p) const
{
	gen_config ret;
	auto path = "part."s+std::string(p);
	ret.map_tmpl = setts.get<std::pmr::string>(path+".file"s);
	ret.tmpl_file = setts.get<std::pmr::string>(path+".tmpl"s);
	ret.split_by_version = setts.get(path+".split_by_version", false);
	conf_links(path, ret);
	return ret;
}

void ptsetts::conf_links(const std::string& path, gen_config& cfg) const
{
	cfg.links.clear();
	auto incs = setts.get_child_optional(path);
	if(incs) for(auto& ip:*incs) {
		if(ip.first == "inc_part") // cannot access the last key
			cfg.links.emplace_back(ip.second.get_value<std::string>());
	}
}

gen_utils::tree ptsetts::generic_ast(std::string_view p) const
{
	struct dslm : gen_utils::dsl_manager {
		boost::json::value psetts;
		dslm(boost::property_tree::ptree p)
		{
			std::stringstream ss;
			boost::property_tree::write_json(ss, p);
			psetts = boost::json::parse(ss.str());
		}
		std::string_view id() const override {return "ptsetts";}
		boost::json::value to_json(
			const gen_utils::compilation_context& cfg,
			const gen_utils::tree& container) const override {
			return psetts;
		}
	};

	struct root_node : gen_utils::data_node {
		std::string_view name() const override {return "ptsetts";}
		std::pmr::vector<gen_utils::name_t> required_links() const override {return {};}
		std::string_view link_condition() const override {return "";}
		std::optional<gen_utils::import_file> imports_modification() const override {return std::nullopt;}

		std::optional<std::uint64_t> version() const override {return 0;}
		std::optional<gen_utils::variable> node_var() const override {return std::nullopt;}
	};

	auto path = "part." + std::string(p);
	boost::property_tree::ptree pchild = setts.get_child(std::string(path));
	gen_utils::tree ret(std::make_shared<root_node>(), std::make_shared<dslm>(std::move(pchild)));
	return ret;
}
