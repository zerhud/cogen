/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include <nlohman/json.hpp>
#include <boost/property_tree/ptree.hpp>
#include "declarations.hpp"

namespace modegen::pg {

/// lang compiler.. compiles to data tree (json)
/// modules (or other inputs) for it can be used in template
class output_descriptor {
public:
	virtual ~output_descriptor() noexcept =default ;

	virtual output_lang lang() const =0 ;
	virtual void override_setts(boost::property_tree::ptree s) =0 ;
	virtual nlohmann::json data(const part_manager& pman) const =0 ;
	virtual FS::path file() const =0 ;
};

} // namespace modegen::pg

