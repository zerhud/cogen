/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "split_version.hpp"

namespace mg = modegen::generation;
namespace mi = modegen::parser::interface;
namespace mgi = modegen::generation::interface;

mgi::split_version::split_version(bool dry_run)
	: dry(dry_run)
{
}

std::vector<mi::module>& mgi::split_version::operator () (std::vector<mi::module>& mods)
{
	result.clear();
	if(dry) return mods;

	for(auto& m:mods) split_mod(std::move(m));
	result.swap(mods);

	return mods;
}

void mgi::split_version::split_mod(mi::module mod)
{
}

