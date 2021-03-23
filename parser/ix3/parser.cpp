/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <fstream>

#include "parse.hpp"
#include "parser.hpp"
#include "grammar/all.hpp"

using namespace std::literals;

ix3::parser::parser() noexcept =default ;

ix3::parser::parser(include_solver is) : solve_inc(std::move(is)) {}

ix3::ast::file_content ix3::parser::parse_stream(std::istream& input) const
{
	input >> std::noskipws ;
	std::istream_iterator<char> end;
	std::istream_iterator<char> begin(input);

	// cannot use single_pass_iterator in parse :(
	std::string data;
	while(begin!=end) data.push_back(*begin++); 

	text::cerr_throw_env env;
	return text::parse(text::file_content, data, env);
}

void ix3::parser::parse(std::istream& input, std::string fn)
{
	ast::file_content result = parse_stream(input);

	result.path = fn;
	if(result.path.is_relative())
		result.path = std::filesystem::absolute(result.path);

	auto frem_fnc = [this](char*){cur_dir.pop_back();};
	std::unique_ptr<char,decltype(frem_fnc)> frem(nullptr,frem_fnc);
	cur_dir.emplace_back(result.path.parent_path());

	if(already_loaded(result.path)) return;
	loaded_files.emplace_back(result.path);

	for(auto& i:result.includes) parse(search_file(i.path));

	checker(std::move(result));
}

void ix3::parser::parse(std::filesystem::path file)
{
	if(!file.is_absolute()) throw std::runtime_error("cannot load relative path"s);
	if(!std::filesystem::exists(file)) throw std::runtime_error("file "s + file.string() + " doesn't exists"s);

	if(already_loaded(file)) return;

	std::fstream file_stream(file.generic_string());
	parse(file_stream, file.generic_string());
}

void ix3::parser::finish_loads()
{
	if(!finished_ast.empty()) return;

	cur_dir.clear();
	finished_ast = checker.extract_result();
}

std::vector<ix3::ast::module> ix3::parser::result() const
{
	return finished_ast;
}

bool ix3::parser::already_loaded(const std::filesystem::path& p) const
{
	for(auto& i:loaded_files) if(i==p) return true;
	return false;
}

std::filesystem::path ix3::parser::search_file(const std::filesystem::path& f) const
{
	for(auto i=cur_dir.rbegin();i!=cur_dir.rend();++i) {
		assert(i->is_absolute());
		auto cur_file = *i / f;
		if(std::filesystem::exists(cur_file)) return cur_file;
	}

	if(solve_inc) return solve_inc(f);

	throw std::runtime_error("file " + f.string() + " not found");
}
