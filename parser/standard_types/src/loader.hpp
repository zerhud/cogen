/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <filesystem>
#include "utils/tree.hpp"

namespace std_types { 

class loader {
public:
	gen_utils::tree load_types(std::filesystem::path file) const ;
	gen_utils::tree load_types(std::istream& src) const ;
};

} // namespace std_types 
