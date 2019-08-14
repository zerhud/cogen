/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "errors.h"

namespace mpr = modegen::parser::errors;

using namespace std::literals;

mpr::error::error(std::string w) noexcept
    : what_(std::move(w))
{
}

mpr::error::error(std::string where, std::string w)
    : what_(std::move(where))
{
	what_ += ": " + std::move(w);
}

const char *mpr::error::what() const noexcept
{
	return what_.c_str();
}

mpr::error_info::error_info(std::string f, std::string p, std::string w)
    : errors::error(f + " " + p + ": ", w)
    , file(std::move(f))
    , path(std::move(p))
    , what(std::move(w))
{
}
