/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "output_descriptor.hpp"
#include "part_algos/module.hpp"
#include "options.hpp"

namespace modegen::pg::outputs {

class cpp : public output_descriptor {
	struct inc_info {
		inc_info(std::string n, bool s) : name(std::move(n)), sys(s) {}
		std::string name;
		bool sys;
		bool is_cpp=true;
	};

	FS::path out_file_;
	nlohmann::json data_;
	std::vector<part_algos_ptr> imngs_;
	const options::part_view opts_;

	std::vector<inc_info> solve_part_includes(const std::string& pname, const part_manager& pman) const ;
	std::vector<inc_info> includes(const std::vector<std::string> sys, const part_manager& pman) const ;
	void add_extra_namespaces(nlohmann::json& cdata) const ;
	void set_constructors_prefix(nlohmann::json& cdata) const ;
public:
	cpp(provider_const_ptr p, FS::path o, const part_descriptor& part);

	output_lang lang() const override ;
	void override_setts(boost::property_tree::ptree s) override ;
	nlohmann::json data(const part_manager& pman) const override ;
	FS::path file() const override ;
};

} // namespace modegen::pg::outputs

