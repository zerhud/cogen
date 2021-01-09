/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "input/tree.hpp"

namespace std_types {

class base_node : public gen_utils::data_node {
public:
	virtual boost::json::value to_json(const gen_utils::tree& con) const =0 ;
};

class root_node : public base_node {
public:
	std::string_view name() const override ;
	std::pmr::vector<gen_utils::name_t> required_links() const override ;
	std::string_view link_condition() const override ;
	std::optional<gen_utils::import_file> imports_modification() const override ;

	std::optional<std::uint64_t> version() const override ;
	std::optional<gen_utils::variable> node_var() const override ;

	boost::json::value to_json(const gen_utils::tree& con) const override ;
};

class type_node : public base_node {
	std::string type;
	std::string target;
	std::string replacer;
	std::pmr::string to_file;
	bool is_sys;
public:
	type_node(
	        std::string name,
	        std::string tgt,
	        std::string rpl,
	        std::pmr::string f,
	        bool sys);

	std::string_view name() const override ;
	std::pmr::vector<gen_utils::name_t> required_links() const override ;
	std::string_view link_condition() const override ;
	std::optional<gen_utils::import_file> imports_modification() const override ;

	std::optional<std::uint64_t> version() const override ;
	std::optional<gen_utils::variable> node_var() const override ;

	boost::json::value to_json(const gen_utils::tree& con) const override ;
};

} // namespace std_types
