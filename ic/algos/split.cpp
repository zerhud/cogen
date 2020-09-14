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
        std::shared_ptr<modegen::ic::input_node> node)
{
	if(node->level()==0)
		all_data->add({node});
	else add_to_root(std::move(node));
}

void modegen::ic::algos::split::add_to_root(std::shared_ptr<input_node> node)
{
	auto roots = all_data->children(nullptr);
	for(auto& root:roots)
		if(node->version() <= root->version()) {
			all_data->add(root, {node});
		}
}
