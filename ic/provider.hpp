/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <filesystem>
#include <boost/json.hpp>
#include "declarations.hpp"

namespace mdg::ic {

class provider {
public:
	virtual ~provider() noexcept =default;

	virtual void generate(
	    std::filesystem::path tmpl_file,
	    const boost::json::value& data,
	    std::string_view out_file) const =0 ;
};

} // namespace mdg::ic
