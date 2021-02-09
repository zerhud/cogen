/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <filesystem>
#include <boost/property_tree/ptree.hpp>

#include "common_utils/input/tree.hpp"
#include "ic/single_gen_part.hpp"

namespace builders {

class loader {
public:
	std::optional<gen_utils::tree> operator()(
	        boost::property_tree::ptree setts,
	        const mdg::ic::gen_context& ctx) const ;
};

} // namespace builders
