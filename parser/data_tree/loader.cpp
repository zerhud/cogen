/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "loader.hpp"

#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std::literals;
namespace md = modegen::parser::data_tree;

md::loader_impl::loader_impl(data_format df)
	: cur_format(df)
{
	// check if all enum entries are used
	assert( cur_format == data_format::info || cur_format == data_format::json );
}

void md::loader_impl::load(std::istream& input, std::string fn)
{
	if(cur_format==data_format::info) boost::property_tree::read_info(input, result);
	else if(cur_format==data_format::json) boost::property_tree::read_json(input, result);
}

void md::loader_impl::load(FS::path file)
{
	TODO("merge result, this operation will clear it")
	if(cur_format==data_format::info) boost::property_tree::read_info(file.u8string(), result);
	else if(cur_format==data_format::json) boost::property_tree::read_json(file.u8string(), result);
}

void md::loader_impl::finish_loads()
{
	TODO("merge results here or from load functions?")
}

boost::property_tree::ptree md::loader_impl::boost_ptree() const
{
	return result;
}

