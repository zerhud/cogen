/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include <fstream>

#include "loader.hpp"
#include "parse.hpp"
#include "grammar/all.hpp"

using namespace std::literals;

ix3::loader::loader() noexcept =default ;

ix3::loader::loader(std::vector<std::filesystem::path> incs)
	: includes_dir(std::move(incs))
{
	for(auto& id:includes_dir) {
		if(id.is_relative()) id = std::filesystem::absolute(id);
	}
}

ix3::ast::file_content ix3::loader::parse_stream(std::istream& input) const
{
	std::istream_iterator<char> end;
	std::istream_iterator<char> begin(input);

	// cannot use single_pass_iterator in parse :(
	std::string data;
	while(begin!=end) data.push_back(*begin++); 

	return text::parse(text::file_content, data, text::parser_env{});
}

void ix3::loader::load(std::istream& input, std::string fn)
{
	ast::file_content result = parse_stream(input);

	result.path = fn;
	if(result.path.is_relative())
		result.path = std::filesystem::absolute(result.path);

	auto frem_fnc = [this](char*){cur_dir.pop_back();};
	std::unique_ptr<char,decltype(frem_fnc)> frem(nullptr,frem_fnc);
	cur_dir.emplace_back(result.path.parent_path());

	loaded_files.emplace_back(result.path);

	for(auto& i:result.includes) load(search_file(i.path));

	checker(std::move(result));
}

void ix3::loader::load(std::filesystem::path file)
{
	if(!file.is_absolute()) throw std::runtime_error("cannot load relative path"s);
	if(!std::filesystem::exists(file)) throw std::runtime_error("file "s + file.string() + " doesn't exists"s);

	if(already_loaded(file)) return;

	std::fstream file_stream(file.generic_u8string());
	load(file_stream, file.generic_u8string());
}

void ix3::loader::finish_loads()
{
	if(cur_dir.empty()) return;
	cur_dir.clear();
	finished_ast = checker.extract_result();
}

std::vector<ix3::ast::module> ix3::loader::result() const
{
	return finished_ast;
}

bool ix3::loader::already_loaded(const std::filesystem::path& p) const
{
	for(auto& i:loaded_files) if(i==p) return true;
	return false;
}

std::filesystem::path ix3::loader::search_file(const std::filesystem::path& f) const
{
	for(auto i=cur_dir.rbegin();i!=cur_dir.rend();++i) {
		assert(i->is_absolute());
		auto cur_file = *i / f;
		if(std::filesystem::exists(cur_file)) return cur_file;
	}

	for(const auto& i:includes_dir) {
		assert(i.is_absolute());
		auto cur_file = i/f;
		if(std::filesystem::exists(cur_file)) return cur_file;
	}

	throw std::runtime_error("file " + f.generic_u8string() + " not found");
}

