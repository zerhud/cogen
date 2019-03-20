/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <nlohman/json.hpp>
#include "file_data.hpp"

namespace modegen::generation {

class python: public file_data {
public:
	nlohmann::json jsoned_data(const std::vector<parser::loader_ptr>& data_loaders, options::view opts) const override ;
	nlohmann::json jsoned_data(const output_info& outputs) const override;
private:
	struct inc_info {
		inc_info(std::string n, bool s) : name(std::move(n)), sys(s) {}
		std::string name;
		bool sys;
	};
};

} // namespace modegen::generation
