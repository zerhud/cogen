/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "meta.hpp"

using namespace std::literals;

bool ix3::ast::meta::operator == (const version& left, const version& right) noexcept
{
	return left.major_v == right.major_v && left.minor_v == right.minor_v;
}

std::string_view ix3::ast::meta::version::name() const { return "versin"sv; }
std::string_view ix3::ast::meta::depricated::name() const { return "depricated"sv; }
std::string_view ix3::ast::meta::documentation::name() const { return "documentation"sv; }
std::string_view ix3::ast::meta::oapi::name() const { return "oapi"sv; }

std::string ix3::ast::meta::version::value() const
{
	return std::to_string(major_v) + '.' + std::to_string(minor_v);
}

std::string ix3::ast::meta::depricated::value() const
{
	std::string ret = "since " + since.value();
	if(!message.empty()) ret += ": " + message;
	return ret;
}

std::string ix3::ast::meta::documentation::value() const
{
	return body;
}

std::string ix3::ast::meta::oapi::value() const
{
	return key + ": " + val;
}

ix3::ast::meta::set& ix3::ast::meta::set::operator =
(std::vector<ix3::ast::meta::set::parameter_t> v) noexcept
{
	cnt = std::move(v);
	return *this;
}
