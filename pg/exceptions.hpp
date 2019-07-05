/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <string>
#include <exception>

namespace modegen::pg::errors {

class error : public std::exception {
	std::string mes;
public:
	error(std::string m);
	const char* what() const noexcept override ;
};

} // namespace modegen::pg::errors

