/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <boost/json.hpp>
#include "declarations.hpp"

namespace gen_utils {

struct import_info {
	std::pmr::string name;
	std::pmr::string cond;
	node_pointer node;
};

class compiled_result final {
	boost::json::value data_;
	std::pmr::vector<import_info> imports_;
	bool import_exists(const import_info& src) const ;
public:
	void combine(compiled_result other);

	void add_import(import_info i) ;
	std::pmr::vector<import_info> imports() const ;

	boost::json::value data() const ;
	void data(boost::json::value d) ;
};

} // namespace gen_utils
