#include "loader.h"

#include <fstream>
#include "grammar.hpp"

modegen::loader::loader() : loader(".", {})
{
}

modegen::loader::loader(std::filesystem::path main_dir, std::vector<std::filesystem::path> includes)
    : cur_dir(std::move(main_dir))
    , incs(std::move(includes))
{
}

void modegen::loader::load(std::istream &input, std::string fn)
{
	std::string pdata{std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
	auto pfile = parse(pdata);
	pfile.path = std::move(fn);

	for(auto& f:pfile.includes) load(f);
	pfile.includes.clear();

	ch(pfile);
}

void modegen::loader::load(std::filesystem::path file)
{
	std::fstream file_stream(file.generic_u8string());
	load(file_stream, file.generic_u8string());
}

std::vector<modegen::module> modegen::loader::result()
{
	return ch.extract_result();
}
