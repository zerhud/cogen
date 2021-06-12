/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <memory>
#include <vector>
#include <filesystem>
#include <iostream>
#include "sddl_export.h"
#include "utils/generic_dsl.hpp"

extern "C"  SDDL_EXPORT
gen_utils::generic_sdl_factory* create_dsl()
{
	struct inner_factory : gen_utils::generic_sdl_factory {
		generic_sdl_container languages(path_solver slv) const override
		{
			return {};
		}
	};

	static inner_factory ret;
	return &ret;
}
