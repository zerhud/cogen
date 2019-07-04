/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "declarations.hpp"

namespace modegen::pg {

class provider {
public:
	virtual ~provider() noexcept =default ;
	virtual part_descriptor_ptr create_part(options::part_view&& ps) const =0 ;
};

} // namespace modegen::pg

