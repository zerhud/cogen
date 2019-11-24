/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "part_manager.hpp"
#include "output_configurator.hpp"
#include "exceptions.hpp"

namespace mpg = modegen::pg;
using namespace std::literals;

std::size_t mpg::part_manager::count() const
{
	return store.size();
}

modegen::pg::part_manager& mpg::part_manager::add(mpg::part_descriptor_ptr p)
{
	store.emplace_back(std::move(p));
	return *this;
}

modegen::pg::part_descriptor_ptr modegen::pg::part_manager::require(std::string_view name) const
{
	for(auto& p:store) if(p->name()==name) return p;
	throw errors::error("part "s + std::string(name) + " not foud"s);
}

std::vector<mpg::part_descriptor_ptr> mpg::part_manager::list() const
{
	return store;
}

std::vector<modegen::pg::output_descriptor_ptr> modegen::pg::part_manager::list_output() const
{
	std::vector<output_descriptor_ptr> ret;
	for(auto& p:store) for(auto& o:p->outputs()) ret.emplace_back(std::move(o));
	for(auto& o:ret) if(!o) throw errors::error("no output in some part");
	return ret;
}

