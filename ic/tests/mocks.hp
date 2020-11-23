/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <turtle/mock.hpp>
#include "ic/core.hpp"

namespace mic = modegen::ic;

namespace icmocks {

MOCK_BASE_CLASS( provider, modegen::ic::provider )
{
	MOCK_METHOD(generate, 3)
};

MOCK_BASE_CLASS( configuration, modegen::ic::configuration )
{
	MOCK_METHOD(parts, 0)

	MOCK_METHOD(all_dsl, 0)

	MOCK_METHOD(split_versions, 1)
	MOCK_METHOD(naming, 1)
	MOCK_METHOD(tmpl_file, 1)
	MOCK_METHOD(map_tmpl, 1)
	MOCK_METHOD(lang, 1)
};

} // namespace icmocks
