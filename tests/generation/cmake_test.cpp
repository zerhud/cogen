#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE cmake

#include <boost/test/unit_test.hpp>

#include "errors.h"
#include "generation/cmake.hpp"

using namespace std::literals;
namespace mg = modegen::generation;

BOOST_AUTO_TEST_CASE(add_library)
{
	boost::property_tree::ptree opts;
	opts.put("gen.cmake.project", "proj");
	opts.put("gen.cmake.libraries.interface.part", "part");
	opts.put("gen.cmake.libraries.interface.file", "some_file.cpp");
	opts.put("gen.part.output", "part_file.cpp");
	mg::options_view opts_view(opts, "cmake"sv);

	mg::cmake cm;
	auto data = cm.jsoned_data(nullptr, opts_view);
	std::vector<std::string> lib_names;
	for(auto& l:data["libraries"].object()) lib_names.emplace_back(l.first);
	BOOST_REQUIRE_EQUAL(lib_names.size(), 1);
	BOOST_CHECK_EQUAL(lib_names[0], "interface"sv);

	BOOST_REQUIRE_EQUAL(data["libraries"]["interface"]["files"].type(), cppjson::is_array);
	BOOST_REQUIRE_EQUAL(data["libraries"]["interface"]["files"].array().size(), 2);
	BOOST_CHECK_EQUAL(data["libraries"]["interface"]["files"][0], "part_file.cpp");
	BOOST_CHECK_EQUAL(data["libraries"]["interface"]["files"][1], "some_file.cpp");
}
