/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "env.hpp"

ix3::text::cerr_throw_env::cerr_throw_env(std::string fn)
    : fn(std::move(fn))
{
}

std::string ix3::text::cerr_throw_env::file_name() const
{
	return fn;
}

void ix3::text::cerr_throw_env::on_error() const
{
	throw std::runtime_error("cannot parser");
}

void ix3::text::cerr_throw_env::on_error(const std::string& message) const
{
	throw std::runtime_error(message);
}

std::ostream& ix3::text::cerr_throw_env::out() const
{
	return std::cerr;
}

std::string ix3::text::cerr_throw_env::msg(const std::string& which) const
{
	return "Error! Expecting: " + which + " here:";
}
