#include "cmake.hpp"
#include <boost/property_tree/json_parser.hpp>

using namespace std::literals;
namespace mg = modegen::generation;
using boost::property_tree::ptree;

cppjson::value mg::cmake::jsoned_data(parser::loader_ptr data_loader, options_view opts) const
{
	cppjson::value data;
	data["project"] = opts.part_data().get<std::string>("project");

	ptree part_data = opts.part_data();
	auto libs = part_data.get_child_optional("libraries");
	for(auto l:*libs) {
		std::vector<std::string> files;
		for(auto& f:l.second) {
			if(f.first=="file"sv) files.emplace_back(f.second.get_value<std::string>());
		}


		data["libraries"][l.first]["files"] = files;
	}

	return data;
}
