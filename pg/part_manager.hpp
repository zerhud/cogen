/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <regex>
#include <vector>
#include "declarations.hpp"

namespace modegen::pg {

/// info file representation, for man how edit this file - configurator.
class part_manager {
	std::vector<part_descriptor_ptr> store;
public:
	std::size_t count() const ;
	part_manager& add(part_descriptor_ptr p);
	part_descriptor_ptr require(std::string_view name) const ;
	std::vector<part_descriptor_ptr> list() const ;
	std::vector<output_descriptor_ptr> list_output() const ;
};

} // namespace modegen::pg

