/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "ix3/ast/file.hpp"

namespace ix3::utils {

template<class T> inline std::enable_if_t<std::is_enum_v<T>,T> operator~ (T a)      { return (T)(~(int)a); }
template<class T> inline std::enable_if_t<std::is_enum_v<T>,T> operator| (T a, T b) { return (T)((int)a | (int)b); }
template<class T> inline std::enable_if_t<std::is_enum_v<T>,T> operator& (T a, T b) { return (T)((int)a & (int)b); }
template<class T> inline std::enable_if_t<std::is_enum_v<T>,T> operator^ (T a, T b) { return (T)((int)a ^ (int)b); }
template<class T> inline std::enable_if_t<std::is_enum_v<T>,T&> operator|= (T& a, T b) { return (T&)((int&)a |= (int)b); }
template<class T> inline std::enable_if_t<std::is_enum_v<T>,T&> operator&= (T& a, T b) { return (T&)((int&)a &= (int)b); }
template<class T> inline std::enable_if_t<std::is_enum_v<T>,T&> operator^= (T& a, T b) { return (T&)((int&)a ^= (int)b); }


enum class selector {
	  function = 1 << 0
	, enumeration = 1 << 1
	, record = 1 << 2
	, interface = 1 << 3
	, all = ~0
};

bool is_selected(const ast::module_content& cnt, selector s);

std::ostream& operator << (std::ostream& out, selector s);

} // namespace ix3::utils
