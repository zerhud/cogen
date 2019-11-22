/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "loader.hpp"
#include "parse.hpp"
#include "grammar/all.hpp"

ix3::loader::loader() noexcept =default ;

ix3::loader::loader(std::vector<std::filesystem::path> incs)
	: includes_dir(std::move(incs))
{
	for(auto& id:includes_dir) {
		if(id.is_relative()) id = std::filesystem::absolute(id);
	}
}


void ix3::loader::load(std::istream& input, std::string fn)
{
	std::istream_iterator<char> end;
	std::istream_iterator<char> begin(input);

	// cannot use single_pass_iterator in parse :(
	std::string data;
	while(begin!=end) data.push_back(*begin++); 

	ast::file_content result = text::parse(text::file_content, data, text::parser_env{});
}

void ix3::loader::load(std::filesystem::path file)
{
}

void ix3::loader::finish_loads()
{
}

std::vector<ix3::ast::module> ix3::loader::result() const
{
	return finished_ast;
}

