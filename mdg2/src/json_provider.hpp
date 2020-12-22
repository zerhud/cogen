/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ic/core.hpp"

namespace mdg2 {

class json_provider : public modegen::ic::provider {
	mutable boost::json::array all_data;
public:
	void generate(
	        std::filesystem::path tmpl_file,
	        const boost::json::value& data,
	        std::string_view out_file) const override ;
	boost::json::value result() const ;
};

} // namespace mdg2

