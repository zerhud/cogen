/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once
#include <boost/property_tree/ptree.hpp>
#include "core.hpp"

namespace modegen::ic {

class single_gen_part final {
	std::shared_ptr<provider> outside;
public:
	single_gen_part(std::shared_ptr<provider> p);
	void operator()(boost::property_tree::ptree setts, input alli) const ;
};

} // namespace modegen::ic
