/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "file_data.hpp"

namespace modegen::generation {

class cmake : public file_data {
public:
	nlohmann::json jsoned_data(const output_info& outputs) const override ;
private:
	void files_from_part(std::vector<std::string>& files, part_descriptor* part) const ;
};

} // namespace modegen::generation
