/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "part_descriptor.hpp"

#include <boost/algorithm/string/replace.hpp>

#include "parser/interface/helpers.hpp"
#include "parser/interface/loader.hpp"
#include "parser/data_tree/loader.hpp"
#include "split_version.hpp"
#include "filter.hpp"
#include "errors.h"

using namespace std::literals;
namespace mp = modegen::parser;
namespace mg = modegen::generation;
namespace mi = modegen::generation::interface;
namespace pi = modegen::parser::interface;

const pi::module& mi::part_descriptor::cur_mod() const
{
	assert( cur_pos < filtered_idl_.size() | filtered_idl_.empty() );
	return filtered_idl_[cur_pos];
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

	filter::request freq;
	//freq.mod_name = cur_mod_name();

	std::string versioning = opts_.get_opt<std::string>(options::template_option::versioning).value_or(""s);
	filtered_idl_ = input_idl_;
	filtered_idl_
	        | split_version(versioning != "split"sv)
	        | filter(freq)
	       ;
}

std::string mi::part_descriptor::part_name() const
{
	return std::string(opts_.part());
}

std::string mi::part_descriptor::file_name() const
{
	const auto& mod=cur_mod();
	std::string output_name = opts_.get<std::string>(mg::options::part_option::output);
	boost::algorithm::replace_all(output_name, "%mod%", mod.name);
	boost::algorithm::replace_all(output_name, "%vm%", std::to_string(pi::get_version(mod).major_v));
	boost::algorithm::replace_all(output_name, "%vi%", std::to_string(pi::get_version(mod).minor_v));
	return output_name;
}

const mg::options::view& mi::part_descriptor::opts() const
{
	return opts_;
}

bool mi::part_descriptor::need_output() const
{
	return !filtered_idl_.empty();
}

bool mi::part_descriptor::next()
{
	if(cur_pos+1 < filtered_idl_.size()) return ++cur_pos;
	return false;
}

std::vector<mp::interface::module> mi::part_descriptor::idl_input() const
{
	assert( cur_pos < filtered_idl_.size() || filtered_idl_.empty() );
	if(filtered_idl_.empty()) return {};
	return {filtered_idl_[cur_pos]};
}

boost::property_tree::ptree mi::part_descriptor::data_input() const
{
	return input_data_;
}
