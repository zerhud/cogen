/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "options.hpp"
#include "declaration.hpp"
#include "parser/loader.hpp"
#include "parser/interface/modegen.hpp"

namespace modegen {
namespace generation {

class part_descriptor {
public:
	virtual ~part_descriptor() noexcept =default ;
	virtual std::string part_name() const =0 ;
	virtual std::string file_name() const =0 ;
	virtual const options::view& opts() const =0 ;
	virtual bool need_output() const =0 ;
	virtual bool next() =0 ;

	virtual std::vector<parser::interface::module> idl_input() const =0 ;
	virtual boost::property_tree::ptree data_input() const =0 ;
};

class single_part_descriptor : public part_descriptor {
	options::view opts_;
	boost::property_tree::ptree input_data_;
	std::vector<parser::interface::module> input_idl_;
public:
	single_part_descriptor(options::view o, std::vector<parser::loader_ptr> plist);
	~single_part_descriptor() noexcept override ;
	std::string part_name() const override ;
	std::string file_name() const override ;
	const options::view& opts() const override ;
	bool need_output() const override ;
	bool next() override ;

	std::vector<parser::interface::module> idl_input() const override ;
	boost::property_tree::ptree data_input() const override ;
};

} // namespace generation
} //  namespace modegen
