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
	std::size_t cur_pos = 0;
	boost::property_tree::ptree input_data_;
	std::vector<parser::interface::module> input_idl_;

	std::string cur_mod_name() const ;
public:
	part_descriptor(options::view o, std::vector<parser::loader_ptr> ldrs);

	std::string part_name() const override ;
	std::string file_name() const override ;
	const options::view& opts() const override ;
	bool need_output() const override ;
	bool next() override ;

	std::vector<parser::interface::module> idl_input() const override ;
	boost::property_tree::ptree data_input() const override ;
};

} // namespace modegen::generation::interface
