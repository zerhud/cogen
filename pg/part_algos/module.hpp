/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "../configuration_algos.hpp"
#include "parser/interface/loader.hpp"

namespace modegen::pg::palgos {

class interface_conf_algos : public configuration_algos {
	std::string tmpl_;
	std::vector<parser::interface::module> mods_;
	std::map<std::string,std::vector<parser::interface::module>> mapped_;

	bool replace(std::string& tmpl, const std::string& var_name, const std::string& value) const ;
	std::map<std::string,std::vector<parser::interface::module>> inner_map(const std::string& tmpl) const ;
	std::string require_data(const parser::interface::module& mod) const ;
public:
	interface_conf_algos(const std::vector<modegen::parser::loader_ptr>& ldrs);
	void set_filter(const options::part_view& pinfo) override ;
	std::vector<std::string> map(const std::string& tmpl) const override ;
	std::map<std::string,std::any> map_to_str(const std::string& tmpl) ;
	mapped_data map_to(mapped_data md) override ;
	std::map<std::string,std::vector<std::string>> map_from(const std::string& tmpl) override ;
	std::vector<modegen::parser::interface::module> mods() const ;
};

} // namespace modegen::pg::palgos


