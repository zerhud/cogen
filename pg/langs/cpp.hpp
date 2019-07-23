/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "pg/output_descriptor.hpp"
#include "pg/part_algos/module.hpp"

namespace modegen::pg::outputs {

class cpp : public output_descriptor {
	FS::path out_file_;
	nlohmann::json data_;
	std::vector<part_algos_ptr> imngs_;
public:
	cpp(provider_const_ptr p, FS::path o, const part_descriptor& part);

	output_lang lang() const override ;
	void override_setts(boost::property_tree::ptree s) override ;
	nlohmann::json data() const override ;
	FS::path file() const override ;
};

} // namespace modegen::pg::outputs

