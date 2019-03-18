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
	std::vector<output_info> jsoned_data(const std::vector<parser::loader_ptr>& data_loaders, options::view opts) const override ;
};

} // namespace modegen::generation
