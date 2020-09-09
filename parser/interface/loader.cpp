/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "loader.hpp"

#include <fstream>
#include "grammar.hpp"

#include "errors.h"

using namespace std::literals;

modegen::parser::interface::loader_impl::loader_impl() : loader_impl(std::vector<FS::path>{})
{
}

modegen::parser::interface::loader_impl::loader_impl(std::vector<FS::path> includes)
    : incs(std::move(includes))
{
	for(auto& i:incs) if(i.is_relative()) i = FS::absolute(i);
}

void modegen::parser::interface::loader_impl::load(std::istream &input, std::string fn)
{
	std::string pdata{std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
	auto pfile = parse(pdata);
	pfile.path = std::move(fn);

	for(auto& f:pfile.includes) load(search_file(std::move(f)));
	pfile.includes.clear();

	ch(pfile);
}

void modegen::parser::interface::loader_impl::load(FS::path file)
{
	if(!file.is_absolute()) throw errors::error("cannot load relative path");
	if(!FS::exists(file)) throw errors::error("cannot load file "s+file.string());

	if(already_loaded(file)) return;
	loaded_files.emplace_back(file);

	auto frem_fnc = [this](char*){cur_dir.pop_back();};
	std::unique_ptr<char,decltype(frem_fnc)> frem(nullptr,frem_fnc);
	cur_dir.emplace_back(file.parent_path());

	std::fstream file_stream(file.generic_string());
	load(file_stream, file.generic_string());
}

void modegen::parser::interface::loader_impl::finish_loads()
{
	if(cur_dir.empty()) return;
	cur_dir.clear();
	result_cache = ch.extract_result();
}

std::vector<modegen::parser::interface::module> modegen::parser::interface::loader_impl::result() const
{
	return result_cache;
}

bool modegen::parser::interface::loader_impl::already_loaded(const FS::path f) const
{
	for(auto& i:loaded_files) if(i==f) return true;
	return false;
}

FS::path modegen::parser::interface::loader_impl::search_file(FS::path f) const
{
	for(auto i=cur_dir.rbegin();i!=cur_dir.rend();++i) {
		assert(i->is_absolute());
		auto cur_file = *i / f;
		if(FS::exists(cur_file)) return cur_file;
	}

	for(const auto& i:incs) {
		assert(i.is_absolute());
		auto cur_file = i/f;
		if(FS::exists(cur_file)) return cur_file;
	}

	throw errors::error("file "s + f.generic_string() + " not found"s);
}

