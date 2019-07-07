/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include <nlohman/json.hpp>
#include "declarations.hpp"

namespace modegen::pg {

/// lang compiler.. compiles to data tree (json)
/// modules (or other inputs) for it can be used in template
class output_descriptor {
public:
	virtual ~output_descriptor() noexcept =default ;

	virtual nlohmann::json data() const =0 ;
	virtual FS::path file() const =0 ;
};

} // namespace modegen::pg


