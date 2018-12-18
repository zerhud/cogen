#include "loader.h"

#include <fstream>
#include "grammar.hpp"

#include "errors.h"

modegen::loader::loader() : loader(std::vector<std::filesystem::path>{})
{
}

modegen::loader::loader(std::vector<std::filesystem::path> includes)
    : incs(std::move(includes))
{
	for(auto& i:incs) if(i.is_relative()) i = std::filesystem::absolute(i);
}

void modegen::loader::load(std::istream &input, std::string fn)
{
	std::string pdata{std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
	auto pfile = parse(pdata);
	pfile.path = std::move(fn);

	for(auto& f:pfile.includes) load(search_file(std::move(f)));
	pfile.includes.clear();

	ch(pfile);
}

void modegen::loader::load(std::filesystem::path file)
{
	if(!file.is_absolute()) throw errors::error("cannot load relative path");

	if(already_loaded(file)) return;
	loaded_files.emplace_back(file);

	auto frem_fnc = [this](char*){cur_dir.pop_back();};
	std::unique_ptr<char,decltype(frem_fnc)> frem(nullptr,frem_fnc);
	cur_dir.emplace_back(file.parent_path());

	std::fstream file_stream(file.generic_u8string());
	load(file_stream, file.generic_u8string());
}

std::vector<modegen::module> modegen::loader::result()
{
	cur_dir.clear();
	return ch.extract_result();
}

bool modegen::loader::already_loaded(const std::filesystem::path f) const
{
	for(auto& i:loaded_files) if(i==f) return true;
	return false;
}

std::filesystem::path modegen::loader::search_file(std::filesystem::path f) const
{
	for(auto i=cur_dir.rbegin();i!=cur_dir.rend();++i) {
		assert(i->is_absolute());
		auto cur_file = *i / f;
		if(std::filesystem::exists(cur_file)) return cur_file;
	}

	for(const auto& i:incs) {
		assert(i.is_absolute());
		auto cur_file = i/f;
		if(std::filesystem::exists(cur_file)) return cur_file;
	}

	throw errors::error("file " + f.generic_u8string() + " not found");
}
