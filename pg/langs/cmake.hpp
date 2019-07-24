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

class cmake : public output_descriptor {
	FS::path out_file_;
	nlohmann::json data_;
	std::vector<part_algos_ptr> imngs_;
	const options::part_view opts_;
	void files_from_part(std::vector<std::string>& files, part_descriptor* part) const ;
public:
	cmake(provider_const_ptr p, FS::path o, const part_descriptor& part);

	output_lang lang() const override ;
	void override_setts(boost::property_tree::ptree s) override ;
	nlohmann::json data(const part_manager& pman) const override ;
	FS::path file() const override ;
};

} // namespace modegen::pg::outputs


