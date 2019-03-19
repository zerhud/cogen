/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "output_info.hpp"
#include <cassert>

#include "part_descriptor.hpp"
#include "errors.h"

using namespace std::literals;
namespace mg = modegen::generation;

bool mg::output_info::empty() const
{
	return parts_.empty();
}

std::vector<mg::part_descriptor*> mg::output_info::parts() const
{
	std::vector<part_descriptor*> ret;
	ret.reserve(parts_.size());
	for(auto& p:parts_) ret.emplace_back(p.get());
	return ret;
}

void mg::output_info::add_part(std::unique_ptr<part_descriptor> part)
{
	if(cur_part_) throw errors::error("cannot add part after iterate starts");
	parts_.emplace_back(std::move(part));
}

mg::part_descriptor* mg::output_info::current_part() const
{
	if(!cur_part_) throw errors::error("call next first");
	assert( *cur_part_ < parts_.size() );
	return parts_[*cur_part_].get();
}

mg::part_descriptor* mg::output_info::require(std::string_view name) const
{
	for(auto& p:parts_) if(p->part_name() == name) return p.get();
	throw errors::error("no such part found"s + std::string(name));
}

bool mg::output_info::next()
{
	if(parts_.empty()) return false;
	if(!cur_part_) cur_part_ = 0;
	if(*cur_part_+1 == parts_.size()) return false;
	cur_part_ = *cur_part_ + 1;
	return true;
}
