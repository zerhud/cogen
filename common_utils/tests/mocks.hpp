/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <turtle/mock.hpp>
#include "common_utils/input/tree.hpp"

namespace gen_utils_mocks {

MOCK_BASE_CLASS(data_node, gen_utils::data_node)
{
	MOCK_METHOD(name, 0)
	MOCK_METHOD(version, 0)
	MOCK_METHOD(node_var, 0)
};

} // namespace gen_utils_mocks
