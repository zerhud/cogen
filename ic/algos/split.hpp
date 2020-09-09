/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "../core.hpp"


namespace modegen::ic::algos {

class split final {
	std::shared_ptr<input> all_data;
public:
	split();
	std::shared_ptr<input> roots() const ;
	void operator ()(std::shared_ptr<input_node> root);
};

} // namespace modegen::ic::algos
