/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "generator.hpp"
#include "exceptions.hpp"

namespace mpg = modegen::pg;

mpg::generator::generator(mpg::provider_ptr p, mpg::options::container_ptr s)
	: prov(std::move(p))
	, setts(std::move(s))
{
}

/// builds all parts.
/// after building all part are ready for generate output.
void mpg::generator::build_env()
{
	init_parts();
	build_deps();
}

mpg::part_manager& mpg::generator::parts()
{
	return pman;
}

void mpg::generator::generate(const FS::path& output_dir) const
{
}

void mpg::generator::generate(std::string_view part, std::ostream& out) const
{
}

void mpg::generator::init_parts()
{
}

void mpg::generator::build_deps()
{
}

