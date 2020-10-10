/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "core.hpp"

using namespace std::literals;

modegen::ic::core::core() = default ;

void modegen::ic::core::gen(std::shared_ptr<configuration> config) const
{
	if(!config)
		throw std::runtime_error( "cannot generate output without configuration"s);
	auto parts = config->parts();
	for(auto& part:parts) build(*config, *part);
	for(auto& part:parts) gen(*config, *part);
}

void modegen::ic::core::build(
          const configuration& config
        , generation_part& part) const
{
	part.rename(config.naming(part.id()));
	if(config.split_versions(part.id()))
		part.split_versions();
	part.map_to(config.map_tmpl(part.id()));
}

void modegen::ic::core::gen(
          const configuration& config
        , const generation_part& part) const
{
	auto outs = part.outputs();
	for(auto& out:outs)
		out->gen(config.output_dir(), config.tmpl_information(part.id()));
}
