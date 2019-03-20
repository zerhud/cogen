/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "part_descriptor.hpp"

#include "errors.h"

using namespace std::literals;
namespace mg = modegen::generation;
namespace mp = modegen::parser;

mg::single_part_descriptor::single_part_descriptor(mg::options::view o)
    : opts_(std::move(o))
{
}

mg::single_part_descriptor::~single_part_descriptor() noexcept
{
}
std::string mg::single_part_descriptor::part_name() const
{
	return std::string(opts_.part());
}

std::string mg::single_part_descriptor::file_name() const
{
	auto val = opts_.get_opt<std::string>(options::part_option::output);
	if(!val) throw errors::gen_error("common"s, "no output file provided for "s + std::string(opts_.part()));
	return *val;
}

bool mg::single_part_descriptor::next()
{
	return false;
}

const mg::options::view& mg::single_part_descriptor::opts() const
{
	assert(opts_.part() == part_name());
	return opts_;
}

bool mg::single_part_descriptor::need_output() const
{
	return true;
}
