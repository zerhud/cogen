/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE standard_types_parser

#include <iomanip>
#include <boost/test/included/unit_test.hpp>
#include <common_utils/tests/mocks.hpp>

#include "loader.hpp"

using namespace std::literals;
using boost::property_tree::ptree;

boost::json::value operator "" _bj(const char* d, std::size_t l)
{
	boost::json::parse_options opts{.allow_trailing_commas=true};
	return boost::json::parse(
	            boost::json::string_view(d,l),
	            boost::json::storage_ptr(),
	            opts);
}

BOOST_AUTO_TEST_SUITE(library)
BOOST_AUTO_TEST_SUITE(builder_system)
BOOST_AUTO_TEST_CASE(no_tree)
{
	ptree setts;
	setts.put("tmpl", "tmpl");
	setts.put("file", "CMakeLists.txt");

	builders::loader ldr;
	mdg::ic::gen_context ctx;
	BOOST_TEST(ldr(setts, ctx).has_value() == false);
}
BOOST_AUTO_TEST_CASE(proj_and_lib)
{
	ptree setts;
	setts.put("project", "proj");
	setts.add("libraries.lib1.part", "a");
	setts.add("libraries.lib1.part", "b");
	setts.add("libraries.lib1.dep", "dep1");
	setts.add("libraries.lib1.dep", "dep2");
	setts.add("libraries.lib1.link_lib", "lib1");
	setts.add("libraries.lib1.link_lib", "lib2");

	builders::loader ldr;
	gen_utils::input dsls;
	mdg::ic::gen_context ctx;
	ctx.generated["a"]["file_a1"] = dsls;
	ctx.generated["a"]["file_a2"] = dsls;
	ctx.generated["b"]["file_b0"] = dsls;

	auto result = ldr(setts, ctx);
	BOOST_REQUIRE(result.has_value());
	BOOST_TEST_REQUIRE(result->children(result->root()).size() == 1);
	gen_utils::compilation_context js_ctx;
	BOOST_TEST(result->to_json(js_ctx) == R"({ "project":"proj", "version":"0.0.0.0",
	           "libraries":{
	             "lib1":{
	               "files":["file_a1", "file_a2", "file_b0"],
	               "deps":["dep1", "dep2"],
	               "link_libs":["lib1", "lib2"]
	             }
	           }})"_bj);
}
BOOST_AUTO_TEST_SUITE_END() // builder_system
BOOST_AUTO_TEST_SUITE_END() // library
