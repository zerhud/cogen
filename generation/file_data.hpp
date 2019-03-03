/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <memory>
#include <string_view>
#include <nlohman/json.hpp>
#include <boost/property_tree/ptree.hpp>
#include "parser/loader.hpp"
#include "options.hpp"

namespace modegen::generation {

class file_data {
public:
	virtual ~file_data() noexcept =default ;
	virtual nlohmann::json jsoned_data(const std::vector<parser::loader_ptr>& data_loaders, options::view opts) const =0 ;
};

} // namespace modegen::generation
