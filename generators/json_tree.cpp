#include "json_tree.h"

#include <fstream>
#include <iostream>

#include "to_json.h"

modegen::gen_options modegen::generators::json_tree::options() const
{
	return gopts;
}

void modegen::generators::json_tree::options(modegen::gen_options opts)
{
	gopts = opts;
}

void modegen::generators::json_tree::output(std::filesystem::path o)
{
	output_file = o;
}

void modegen::generators::json_tree::generate(std::vector<modegen::module> mods) const
{
	cast_options(gopts, mods);

	cppjson::value jsoned = modegen::converters::to_json(mods);
	if(output_file.generic_u8string().empty()) std::cout << jsoned << std::endl;
	else {
		std::fstream file(output_file, file.out);
		file << jsoned;
		file.flush();
	}
}
