/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include "declarations.hpp"

namespace modegen::pg {

class output_descriptor {
public:
	virtual ~output_descriptor() noexcept =default ;

	nlohman::json data() const =0 ;
	virtual FS::path file() const =0 ;
};

} // namespace modegen::pg


