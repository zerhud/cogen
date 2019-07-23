/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "parser/interface/modegen.hpp"

namespace modegen::generation::interface {

template<typename Cvt>
std::result_of_t<Cvt(std::vector<modegen::parser::interface::module>&)> operator | (std::vector<modegen::parser::interface::module>& m, Cvt& c)
{
	return c(m);
}

template<typename Cvt>
std::result_of_t<Cvt(std::vector<modegen::parser::interface::module>&)> operator | (std::vector<modegen::parser::interface::module>& m, const Cvt& c)
{
	return c(m);
}

template<typename Cvt>
std::result_of_t<Cvt(std::vector<modegen::parser::interface::module>&)> operator | (std::vector<modegen::parser::interface::module>& m, Cvt&& c)
{
	return c(m);
}

} // namespace modegen::generation::interface
