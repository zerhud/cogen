#include "json_tree.h"

#include <fstream>
#include <iostream>

#include "converters/to_json.h"

void modegen::generators::json_tree::generate(modegen::mod_selection query, std::vector<modegen::module> mods) const
{
	//filter_by_selection(query, mods);

	//cppjson::value jsoned = modegen::cvt::to_json{}(mods);
	//if(!query.output.has_value()) std::cout << jsoned << std::endl;
	//else {
	    //std::fstream file(*query.output, file.out);
	    //file << jsoned;
	    //file.flush();
	//}
}
