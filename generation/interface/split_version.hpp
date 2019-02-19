/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <map>
#include <functional>
#include "parser/interface/modegen.hpp"
#include "parser/interface/helpers.hpp"

namespace modegen::generation::interface {

class split_version {
public:
	split_version(bool dry_run = false);
	std::vector<parser::interface::module>& operator () (std::vector<parser::interface::module>& mods) ;
private:
	void split_mod(parser::interface::module mod);

	bool dry = false;
	std::size_t current_mod = 0;
	std::vector<parser::interface::module> result;
};

} // namespace modegen::generation::interface

