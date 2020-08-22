/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "core.hpp"

modegen::ic::core::core()
{
}

void modegen::ic::core::gen(std::shared_ptr<input> data, std::shared_ptr<output> out, std::shared_ptr<configuration> config) const
{
	if(!data || !out || !config)
		throw std::runtime_error("cannot generate output without input, configuration or provider");
	auto outs = config->outputs();
	for(auto& oi:outs)
		out->add(oi.name, oi.tmpl, data->to_json());
}
