/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "exceptions.hpp"

using namespace std::literals;

namespace mpe = modegen::pg::errors;

mpe::error::error(std::string m) : mes(std::move(m)) {}
const char* mpe::error::what() const noexcept {return mes.c_str();}

mpe::notready::notready(std::string fn) : error(fn + " functionality are not ready yet"s) {}

