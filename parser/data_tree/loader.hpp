/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/property_tree/ptree.hpp>

#include "parser/loader.hpp"

namespace modegen::parser::data_tree {

class loader : public parser::loader {
public:
	virtual boost::property_tree::ptree boost_ptree() const =0 ;
};

class loader_impl : public loader {
public:
	enum class data_format { info, json };
	explicit loader_impl(data_format df);

	void load(std::istream& input, std::string fn) override ;
	void load(FS::path file) override ;
	void finish_loads() override ;
	boost::property_tree::ptree boost_ptree() const override ;
private:
	data_format cur_format;
	boost::property_tree::ptree result;
};

} // namespace modegen::parser::data_tree

