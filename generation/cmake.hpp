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
	cppjson::value jsoned_data(parser::loader_ptr data_loader, options_view opts) const override ;
};

} // namespace modegen::generation
