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

#include "ptree_merge.hpp"

using namespace std::literals;
namespace md = modegen::parser::data_tree;

md::loader_impl::loader_impl(data_format df)
	: cur_format(df)
{
	next_input_format(df);
}

void md::loader_impl::next_input_format(data_format df)
{
	cur_format = df;
	// check if all enum entries are used
	assert( cur_format == data_format::info || cur_format == data_format::json );
}

void md::loader_impl::load(std::istream& input, std::string fn)
{
	boost::property_tree::ptree file;
	if(cur_format==data_format::info) boost::property_tree::read_info(input, file);
	else if(cur_format==data_format::json) boost::property_tree::read_json(input, file);
	files[fn] = std::move(file);
}

void md::loader_impl::load(FS::path file)
{
	boost::property_tree::ptree file_data;
	if(cur_format==data_format::info) boost::property_tree::read_info(file.string(), file_data);
	else if(cur_format==data_format::json) boost::property_tree::read_json(file.string(), file_data);
	files[file.generic_string()] = std::move(file_data);
}

void md::loader_impl::finish_loads()
{
	boost::property_tree::merger m;
	for(auto& [fn, data]:files) m.add(data);
	result = m;
}

boost::property_tree::ptree md::loader_impl::boost_ptree() const
{
	return result;
}

