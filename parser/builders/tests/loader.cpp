/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE builders

#include <iomanip>
#include <boost/test/included/unit_test.hpp>
#include <common_utils/tests/mocks.hpp>

#include "loader.hpp"
#include "utils/map_to.hpp"

using namespace std::literals;
using gen_utils::variable;
using gen_utils_mocks::mk_tree;
using gen_utils_mocks::mk_node;
using boost::property_tree::ptree;
using gen_utils_mocks::trees_fixture;

gen_utils::map_to::result_inputs_t config_manager(
        std::pmr::string part,
        std::pmr::string tmpl,
        gen_utils::imports_manager& mng,
        const gen_utils::input& all_data)
{
	auto ret = gen_utils::map_to()(tmpl, all_data);
	for(auto& [n,d]:ret) mng(part, n,d);
	return ret;
}

BOOST_AUTO_TEST_SUITE(library)
BOOST_AUTO_TEST_SUITE(builder_system)
BOOST_AUTO_TEST_CASE(no_tree)
{
	ptree setts;
	setts.put("tmpl", "tmpl");
	setts.put("file", "CMakeLists.txt");

	builders::loader ldr;
	cogen::ic::gen_context ctx;
	BOOST_TEST(ldr(setts, ctx).has_value() == false);
}
BOOST_FIXTURE_TEST_CASE(proj_and_lib, trees_fixture)
{
	ptree setts;
	setts.put("project", "proj");
	setts.add("import", "lib_a");
	setts.add("import", "lib_b");
	setts.add("libraries.lib1.part", "a");
	setts.add("libraries.lib1.part", "b");
	setts.add("libraries.lib1.dep", "dep1");
	setts.add("libraries.lib1.dep", "dep2");
	setts.add("libraries.lib1.link_lib", "lib1");
	setts.add("libraries.lib1.link_lib", "lib2");

	mk_tree(t1(), {
	            {std::nullopt, {.version=1, .node_var=variable{"a", "a1"}}}
	          , {std::nullopt, {.version=1, .node_var=variable{"a", "a2"}}}
	          , {std::nullopt, {.version=1, .node_var=variable{"b", "b1"}}}
	        });
	gen_utils::input dsls;
	dsls.add(t1());

	builders::loader ldr;
	cogen::ic::gen_context ctx;
	gen_utils::imports_manager imng;
	ctx.generated["a"] = config_manager("a", "file_${a}", imng, dsls);
	ctx.generated["b"] = config_manager("b", "file_${b}", imng, dsls);

	auto result = ldr(setts, ctx);
	BOOST_REQUIRE(result.has_value());
	BOOST_TEST_REQUIRE(result->children(result->root()).size() == 2);
	gen_utils::compilation_context js_ctx{.links = &imng, .all_input = &dsls};
	BOOST_TEST(result->to_json(js_ctx) == R"({ "project":"proj", "version":"0.0.0.0",
	           "imports":["lib_a", "lib_b"],
	           "libraries":{
	             "lib1":{
	               "files":["file_a1", "file_a2", "file_b1"],
	               "deps":["dep1", "dep2"],
	               "link_libs":["lib1", "lib2"]
	             }
	           }})"_bj);
}
BOOST_FIXTURE_TEST_CASE(with_map_from, trees_fixture)
{
	ptree setts;
	setts.put("project", "proj");
	setts.add("libraries.lib_${v}.part", "a");
	setts.add("libraries.lib_${v}.part", "b");
	setts.add("libraries.lib_${v}.dep", "dep1");
	setts.add("libraries.lib_${v}.link_lib", "lib1");

	mk_tree(t1(), {
	            {std::nullopt, {.version=1, .node_var=variable{"v", "v1"}}}
	          , {std::nullopt, {.version=1, .node_var=variable{"v", "v2"}}}
	        });
	gen_utils::input dsls;
	dsls.add(t1());

	cogen::ic::gen_context ctx;
	gen_utils::imports_manager imng;
	ctx.generated["a"] = config_manager("a", "a_${v}", imng, dsls);
	ctx.generated["b"] = config_manager("b", "b_${v}", imng, dsls);
	ctx.generated["c"] = config_manager("c", "c_${v}", imng, dsls);

	builders::loader ldr;
	auto result = ldr(setts, ctx);
	BOOST_REQUIRE(result.has_value());
	BOOST_TEST_REQUIRE(result->children(result->root()).size() == 2);
	gen_utils::compilation_context js_ctx{.links = &imng, .all_input = &dsls};

	auto right_result = R"({ "project":"proj", "version":"0.0.0.0",
	           "libraries":{
	             "lib_v1":{
	               "files":["a_v1", "b_v1"],
	               "deps":["dep1"],
	               "link_libs":["lib1"]
	             },
	             "lib_v2":{
	               "files":["a_v2", "b_v2"],
	               "deps":["dep1"],
	               "link_libs":["lib1"]
	             }
	           }})"_bj;

	BOOST_TEST_CONTEXT("with single tree") {
		BOOST_TEST(result->to_json(js_ctx) == right_result);
	}

	mk_tree(t2(), {{},{}});
	dsls.add(t2());
	ctx.generated.clear();
	ctx.generated["a"] = config_manager("a", "a_${v}", imng, dsls);
	ctx.generated["b"] = config_manager("b", "b_${v}", imng, dsls);
	ctx.generated["c"] = config_manager("c", "c_${v}", imng, dsls);
	result = ldr(setts, ctx);

	BOOST_TEST_CONTEXT("with few trees") {
		BOOST_TEST(result->to_json(js_ctx) == right_result);
	}
}
BOOST_AUTO_TEST_SUITE_END() // builder_system
BOOST_AUTO_TEST_SUITE_END() // library
