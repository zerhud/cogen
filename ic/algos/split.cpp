/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "split.hpp"


modegen::ic::algos::split::split()
    : all_data(std::make_shared<input>())
{
}

std::shared_ptr<modegen::ic::input> modegen::ic::algos::split::roots() const
{
	return all_data;
}

void modegen::ic::algos::split::operator()(
        std::shared_ptr<modegen::ic::input_node> root)
{
	all_data->add({root});
}
