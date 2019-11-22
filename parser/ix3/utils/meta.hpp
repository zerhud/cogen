/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <optional>
#include "ast/common.hpp"

namespace ix3::ast {

template<typename Meta>
std::optional<Meta> get(const meta::set& s)
{
	for(auto& c:s.cnt)
	{
		Meta* m = boost::get<Meta*>(c);
		if(m) return *m;
	}

	return std::nullopt;
}

} // namespace ix3::ast
