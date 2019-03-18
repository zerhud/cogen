/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "part_descriptor.hpp"

#include <boost/algorithm/string/replace.hpp>

#include "parser/interface/loader.hpp"

using namespace std::literals;
namespace mp = modegen::parser;
namespace mg = modegen::generation;
namespace mi = modegen::generation::interface;

mi::part_descriptor::part_descriptor(mg::options::view o, std::vector<mp::loader_ptr> ldrs)
    : opts_(std::move(o))
{
	for(auto& _ldr:ldrs) {
		mp::interface::loader* ldr = dynamic_cast<mp::interface::loader*>(_ldr.get());
		if(!ldr) continue;
		for(auto& mod:ldr->result()) mods_.emplace_back(mod.name);
	}
}

std::string mi::part_descriptor::part_name() const
{
	return std::string(opts_.part());
}

std::string mi::part_descriptor::file_name() const
{
	std::string output_name = opts_.get<std::string>(mg::options::part_option::output);

	if(cur_pos < mods_.size()) {
		auto mname = mods_[cur_pos];
		boost::algorithm::replace_all(output_name, "%mod%", mname);
	}

	return output_name;
}

mg::options::view& mi::part_descriptor::opts()
{
	return opts_;
}

bool mi::part_descriptor::need_output() const
{
	return !mods_.empty();
}

bool mi::part_descriptor::next()
{
	return ++cur_pos < mods_.size();
}
