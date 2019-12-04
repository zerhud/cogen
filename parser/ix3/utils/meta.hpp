/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <optional>
#include "ix3/ast/meta.hpp"

namespace ix3::ast {

template<typename Meta>
std::optional<Meta> get(const meta::set& s)
{
	for(auto& c:s.cnt)
	{
		const Meta* m = boost::get<Meta>(&c.var);
		if(m) return *m;
	}

	return std::nullopt;
}

template<typename Meta>
void set(meta::set& s, Meta val)
{
	for(auto& c:s.cnt)
	{
		Meta* m = boost::get<Meta>(&c.var);
		if(m) {
			*m = std::move(val);
			return;
		}
	}

	s.cnt.emplace_back(std::move(val));
}

namespace meta {

bool operator < (const version& left, const version& right);
bool operator <= (const version& left, const version& right);
bool operator != (const version& left, const version& right);

} // namespace meta

} // namespace ix3::ast
