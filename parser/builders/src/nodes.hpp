/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/property_tree/ptree.hpp>
#include "utils/tree.hpp"
#include "ic/single_gen_part.hpp"

namespace builders {

class base_node : public gen_utils::data_node {
public:
	std::string_view name() const override ;
	std::optional<std::uint64_t> version() const override ;
	std::optional<gen_utils::variable> node_var() const override ;
	std::optional<gen_utils::import_file> imports_modification() const override ;
	std::string_view link_condition() const override ;
	std::pmr::vector<gen_utils::name_t> required_links() const override ;

	virtual boost::json::value to_json(
	        const gen_utils::tree& con,
	        const gen_utils::compilation_context& ctx) const =0;
};

class root_node : public base_node {
public:
	std::string_view name() const override ;
	boost::json::value to_json(
	        const gen_utils::tree& con,
	        const gen_utils::compilation_context& ctx) const override ;
};

class project : public base_node {
	std::string pname;
	std::string ver;
public:
	project(std::string n, std::string v);
	boost::json::value to_json(
	        const gen_utils::tree& con,
	        const gen_utils::compilation_context& ctx) const override ;
};

class library : public base_node {
	std::string lib;
	boost::property_tree::ptree setts;
	std::pmr::map<std::pmr::string,gen_utils::input> mapped;
	std::vector<std::string> files;
	std::vector<std::string> deps;
	std::vector<std::string> libs;

	boost::json::value make_json_deps() const ;
	boost::json::value make_json_libs() const ;
	boost::json::value make_json_files(
	        const std::pmr::vector<std::pmr::string>& list) const ;
public:
	library(
	        std::string n,
	        boost::property_tree::ptree lsetts,
	        const mdg::ic::gen_context& ctx);
	boost::json::value to_json(
	        const gen_utils::tree& con,
	        const gen_utils::compilation_context& ctx) const override ;
};

} // namespace builders
