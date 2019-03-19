/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <optional>
#include "declaration.hpp"

namespace modegen{ namespace generation {

class output_info final {
public:
	bool empty() const ;

	/// all contained parts
	std::vector<part_descriptor*> parts() const ;
	void add_part(std::unique_ptr<part_descriptor> part);

	/// part selected by \ref next call
	part_descriptor* current_part() const ;

	/// try to search part by name
	/// @param name part's name as read from info file
	/// @throws throws errro if not found
	/// @returns always returns found part
	part_descriptor* require(std::string_view name) const ;

	/// interate by parts for get part info from \ref current_part
	/// setts the first part (0 index) on the first call
	/// @returns true if there is more part, false if no part
	bool next() ;
private:
	std::vector<std::unique_ptr<part_descriptor>> parts_;
	std::optional<std::size_t> cur_part_ = 0;
};

}} // namespace modegen::generation
