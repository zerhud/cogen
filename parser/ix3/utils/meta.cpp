/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cppjinja.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "meta.hpp"

bool ix3::ast::meta::operator < (const version& left, const version& right)
{
	return left.major_v < right.major_v && left.minor_v < right.minor_v;
}

bool ix3::ast::meta::operator <= (const version& left, const version& right)
{
	return left.major_v <= right.major_v && left.minor_v <= right.minor_v;
}
