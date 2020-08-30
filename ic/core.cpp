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
	auto parts = config->parts();
	for(auto& part:parts) {
		part->rename(config->naming(part->id()));
		if(config->split_versions(part->id()))
			part->split_versions();
		part->map_to(config->map_tmpl(part->id()));
		auto outs = part->outputs();
		for(auto& out:outs)
			out->gen(config->output_dir(), config->tmpl_information(part->id()));
	}
}
