#include "json_tree.h"

#include <fstream>
#include <iostream>

#include "to_json.h"

void modegen::generators::json_tree::set_option(const std::string &key, const std::string &val)
{
	//TODO: store options..
}

void modegen::generators::json_tree::generate(modegen::mod_selection query, std::vector<modegen::module> mods) const
{
	filter_by_selection(query, mods);

	cppjson::value jsoned = modegen::converters::to_json(mods);
	if(!query.output.has_value()) std::cout << jsoned << std::endl;
	else {
		std::fstream file(*query.output, file.out);
		file << jsoned;
		file.flush();
	}
}
