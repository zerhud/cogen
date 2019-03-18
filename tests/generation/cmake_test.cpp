/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE cmake

#include <boost/test/unit_test.hpp>

#include "errors.h"
#include "generation/cmake.hpp"

using namespace std::literals;
namespace mg = modegen::generation;

BOOST_AUTO_TEST_CASE(add_library)
{
	auto opts = std::make_shared<mg::options::container>();
	opts->raw().put("gen.cmake.output", "cm");
	opts->raw().put("gen.cmake.project", "proj");
	opts->raw().put("gen.cmake.libraries.interface.part", "some_part");
	opts->raw().put("gen.cmake.libraries.interface.file", "some_file.cpp");
	opts->raw().put("gen.some_part.output", "part_file.cpp");
	mg::options::view opts_view(opts, "cmake"s);

	mg::cmake cm;
	auto _data = cm.jsoned_data({}, opts_view);
	BOOST_REQUIRE_EQUAL(_data.size(), 1);
	BOOST_CHECK_EQUAL(_data[0].out_file, "cm");
	auto& data = _data[0].data;

	BOOST_CHECK_EQUAL(data["libraries"].size(), 1);
	for(auto& [k,v]:data["libraries"].items()) BOOST_CHECK_EQUAL(k, "interface"s);

	BOOST_REQUIRE(data["libraries"]["interface"]["files"].is_array());
	BOOST_REQUIRE_EQUAL(data["libraries"]["interface"]["files"].size(), 2);
	BOOST_CHECK_EQUAL(data["libraries"]["interface"]["files"][0], "part_file.cpp");
	BOOST_CHECK_EQUAL(data["libraries"]["interface"]["files"][1], "some_file.cpp");
}
