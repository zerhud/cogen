/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <turtle/mock.hpp>
#include "ic/provider.hpp"
#include "common_utils/tests/mocks.hpp"

namespace mic = cogen::ic;

namespace icmocks {

MOCK_BASE_CLASS( provider, mic::provider )
{
	MOCK_METHOD(generate, 3)
};

} // namespace icmocks
