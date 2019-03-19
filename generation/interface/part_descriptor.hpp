/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "parser/loader.hpp"
#include "generation/part_descriptor.hpp"

namespace modegen::generation::interface {


class part_descriptor : public generation::part_descriptor {
	options::view opts_;
	std::vector<std::string> mods_;
	std::size_t cur_pos = 0;
public:
	part_descriptor(options::view o, std::vector<parser::loader_ptr> ldrs);

	std::string part_name() const override ;
	std::string file_name() const override ;
	const options::view& opts() const override ;
	bool need_output() const override ;
	bool next() override ;
};

} // namespace modegen::generation::interface
