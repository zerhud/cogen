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
	std::vector<std::any> inputs_;
	std::optional<options::part_view> opts_;
public:
	cmake(FS::path o, std::vector<std::any> data);

	output_lang lang() const override ;
	void setts(const options::part_view& s) override ;
	nlohmann::json data(const part_manager& pman) const override ;
	FS::path file() const override ;
};

} // namespace modegen::pg::outputs


