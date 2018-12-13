#include "loader.h"

modegen::loader::loader() : loader(".", {})
{
}

modegen::loader::loader(std::filesystem::path main_dir, std::vector<std::filesystem::path> includes)
    : cur_dir(std::move(main_dir))
    , incs(std::move(includes))
{
}

void modegen::loader::load(std::istream &input)
{
	(void) input;
}

void modegen::loader::load(std::filesystem::path file)
{
	(void) file;
}

std::vector<modegen::module> modegen::loader::result()
{
	std::vector<module> ret;
	ret.swap(accum);
	return ret;
}

std::vector<modegen::module> modegen::loader::copy_result() const
{
	return accum;
}
