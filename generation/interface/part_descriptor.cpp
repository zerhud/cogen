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
#include "parser/data_tree/loader.hpp"
#include "filter.hpp"
#include "errors.h"

using namespace std::literals;
namespace mp = modegen::parser;
namespace mg = modegen::generation;
namespace mi = modegen::generation::interface;

std::string mi::part_descriptor::cur_mod_name() const
{
	assert( cur_pos < input_idl_.size() | input_idl_.empty() );
	return input_idl_[cur_pos].name;
}

mi::part_descriptor::part_descriptor(mg::options::view o, std::vector<mp::loader_ptr> ldrs)
    : opts_(std::move(o))
{
	for(auto& _ldr:ldrs) {
		mp::data_tree::loader* exd_ldr = dynamic_cast<mp::data_tree::loader*>(_ldr.get());
		if(exd_ldr) {
			if(!input_data_.empty()) throw errors::error("extra data loader dublicate");
			input_data_ = exd_ldr->boost_ptree();
			continue;
		}

		mp::interface::loader* idl_ldr = dynamic_cast<mp::interface::loader*>(_ldr.get());
		if(idl_ldr) {
			if(!input_idl_.empty()) throw errors::error("idl loader dublicate");
			input_idl_ = idl_ldr->result();
			continue;
		}
	}
}

std::string mi::part_descriptor::part_name() const
{
	return std::string(opts_.part());
}

std::string mi::part_descriptor::file_name() const
{
	std::string output_name = opts_.get<std::string>(mg::options::part_option::output);
	boost::algorithm::replace_all(output_name, "%mod%", cur_mod_name());
	return output_name;
}

const mg::options::view& mi::part_descriptor::opts() const
{
	return opts_;
}

bool mi::part_descriptor::need_output() const
{
	return !input_idl_.empty();
}

bool mi::part_descriptor::next()
{
	if(cur_pos+1 < input_idl_.size()) return ++cur_pos;
	return false;
}

std::vector<mp::interface::module> mi::part_descriptor::idl_input() const
{
	filter::request freq;
	freq.mod_name = cur_mod_name();
	auto filtered = input_idl_;
	return filtered | filter(freq);
}

boost::property_tree::ptree mi::part_descriptor::data_input() const
{
	return input_data_;
}
