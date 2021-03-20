/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE utils_tree

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "utils/tree.hpp"
#include "mocks.hpp"

using namespace std::literals;
using gen_utils_mocks::trees_fixture;
using gen_utils_mocks::check_vec_eq;

BOOST_AUTO_TEST_SUITE(input)
using gen_utils_mocks::mk_tree;
using gen_utils_mocks::mk_node;

BOOST_AUTO_TEST_SUITE(tree)
using gen_utils::variable;
BOOST_FIXTURE_TEST_CASE(mk_tree_proto_test, trees_fixture)
{
	mk_tree(t1(), {
	            {std::nullopt, {.version=100}},
	            {std::nullopt, {.version=200}},
	            { 0, {.version=110}},
	            { 2, {.version=111}},
	            { 1, {.version=210}},
	            {-1, {.version=211}},
	        });
	auto& t = t1();
	auto& tr = t.root();
	BOOST_TEST(t.children(tr).size() == 2);

	auto c100 = t.children(tr).at(0);
	BOOST_TEST(*c100->version() == 100);
	BOOST_TEST(t.children(*c100).size() == 1);
	auto c110 = t.children(*c100).at(0);
	BOOST_TEST(*c110->version() == 110);
	BOOST_TEST(t.children(*c110).size() == 1);
	BOOST_TEST(*t.children(*c110).at(0)->version() == 111);

	auto c200 = t.children(tr).at(1);
	BOOST_TEST(*c200->version() == 200);
	BOOST_TEST(t.children(*c200).size() == 1);
	auto c210 = t.children(*c200).at(0);
	BOOST_TEST(*c210->version() == 210);
	BOOST_TEST(t.children(*c210).size() == 1);
	auto c211 = t.children(*c210).at(0);
	BOOST_TEST(*c211->version() == 211);
	BOOST_TEST(t.children(*c211).size() == 0);
}
BOOST_AUTO_TEST_CASE(getters)
{
	auto dm = std::make_shared<gen_utils_mocks::dsl_manager>();
	MOCK_EXPECT(dm->id).returns("data_id"sv);

	BOOST_CHECK_THROW(gen_utils::tree(nullptr, dm), std::exception);
	BOOST_CHECK_THROW(gen_utils::tree(nullptr, nullptr), std::exception);

	auto bad_node = std::make_shared<gen_utils_mocks::data_node>();
	MOCK_EXPECT(bad_node->version).returns(std::nullopt);
	BOOST_CHECK_THROW(gen_utils::tree(bad_node, dm), std::exception);

	auto node = std::make_shared<gen_utils_mocks::data_node>();
	BOOST_CHECK_THROW(gen_utils::tree(node, nullptr), std::exception);
	MOCK_EXPECT(node->version).returns(1102);
	gen_utils::tree t(node, dm);
	BOOST_TEST(t.data_id() == "data_id"sv);
	BOOST_TEST(&t.root() == node.get());

	BOOST_TEST(t.root_version() == 1102);
	t.root_version(10);
	BOOST_TEST(t.root_version() == 10);

	gen_utils::compilation_context ctx;
	MOCK_EXPECT(dm->to_json).calls([&t,&ctx](
	        const gen_utils::compilation_context& c, const gen_utils::tree& gt){
		BOOST_TEST(&c == &ctx);
		BOOST_CHECK(&gt == &t);
		boost::json::object ret;
		ret["test"] = "ok";
		return ret; });
	BOOST_TEST(t.to_json(ctx) == boost::json::parse(R"({"test":"ok"})"));
}
BOOST_AUTO_TEST_SUITE(add_children)
BOOST_FIXTURE_TEST_CASE(no_parent, trees_fixture)
{
	auto bad_node = std::make_shared<gen_utils_mocks::data_node>();
	auto bad_node2 = std::make_shared<gen_utils_mocks::data_node>();
	BOOST_CHECK_THROW((void)t1().children(*bad_node), std::exception);
	BOOST_CHECK_THROW(t1().add(*bad_node, bad_node2), std::exception);
}
BOOST_FIXTURE_TEST_CASE(cannot_add_low_version, trees_fixture)
{
	auto bad_node = mk_node({.version=t1().root_version()-1});
	BOOST_CHECK_THROW(t1().add(t1().root(), bad_node), std::exception);
}
BOOST_FIXTURE_TEST_CASE(adding_twice, trees_fixture)
{
	auto bad_node = mk_node({});
	t1().add(t1().root(), bad_node);
	BOOST_CHECK_THROW(t1().add(t1().root(), bad_node), std::exception);
}
BOOST_FIXTURE_TEST_CASE(can_find_child, trees_fixture)
{
	auto child1 = mk_node(t1(), t1().root(), {});
	auto child2 = mk_node(t1(), t1().root(), {});
	auto child12 = mk_node(t1(), *child1, {});
	BOOST_TEST_REQUIRE(t1().children(t1().root()).size()==2);
	BOOST_TEST(t1().children(t1().root())[0]==child1);
	BOOST_TEST(t1().children(t1().root())[1]==child2);
	BOOST_TEST_REQUIRE(t1().children(*child1).size()==1);
	BOOST_TEST(t1().children(*child1)[0]==child12);
}
BOOST_AUTO_TEST_SUITE_END() // add_children
BOOST_FIXTURE_TEST_CASE(next_min_version, trees_fixture)
{
	BOOST_TEST(*t1_root->version()==1);
	BOOST_TEST(t1().next_min_version()==1);
	BOOST_TEST(t1().root_version()==1);

	auto child1 = mk_node(t1(), t1().root(), {.version=110});
	BOOST_TEST(t1().next_min_version()==110);
	BOOST_TEST(t1().root_version()==1);

	mk_node(t1(), t1().root(), {});
	BOOST_TEST(t1().next_min_version()==110);
	BOOST_TEST(t1().root_version()==1);

	mk_node(t1(), *child1, {.version=105});
	BOOST_TEST(t1().next_min_version()==105);
	BOOST_TEST(t1().root_version()==1);

	t1().root_version(106);
	BOOST_TEST(t1().next_min_version()==110);
}
BOOST_AUTO_TEST_SUITE(copy)
BOOST_FIXTURE_TEST_CASE(no_cond, trees_fixture)
{
	t1().add(t1().root(), mk_node({}), mk_node({}));
	auto tcopy = t1().copy_if(gen_utils::tree::copy_condition{});
	BOOST_CHECK( !tcopy.has_value()) ;
}
BOOST_FIXTURE_TEST_CASE(empty_tree, trees_fixture)
{
	auto tc1 = t1().copy_if([](auto &) { return false; });
	t1().add(t1().root(), mk_node({}), mk_node({}));
	auto tc2 = t1().copy_if([](auto &) { return false; });
	BOOST_CHECK( !tc1 );
	BOOST_CHECK( !tc2 );
}
BOOST_FIXTURE_TEST_CASE(few_childs, trees_fixture)
{
	auto nodes = mk_tree(t1(), {
	                         {std::nullopt, {}}, {0, {}}, {0, {}}
	                       , {std::nullopt, {}}
	                     });
	auto child2 = nodes.back();
	auto child12 = nodes[2];

	auto tc = t1().copy_if([child2, child12](auto& n) {
		return !(&n == child2.get() || &n == child12.get());
	});
	BOOST_REQUIRE( tc.has_value()) ;
	BOOST_TEST_REQUIRE(tc->children(tc->root()).size()==1);
	BOOST_TEST(tc->children(tc->root())[0] == nodes[0]);
	BOOST_TEST(tc->children(*nodes[0]).size() == 1);
	BOOST_TEST(tc->children(*nodes[0]).at(0) == nodes[1]);

	auto child121 = mk_node(t1(), *child12, {.version=111});
	mk_node(t1(), *child2, {.version=201});

	tc = t1().copy_if([&nodes](auto& n){ return &n != nodes[0].get(); });
	BOOST_REQUIRE(tc.has_value());
	BOOST_TEST_REQUIRE(tc->children(tc->root()).size()==1);
	BOOST_TEST(tc->children(tc->root())[0] == child2);
	BOOST_CHECK_THROW(tc->children(*nodes[0]).size(), std::exception);
	BOOST_TEST(tc->children(*child2).size() == 1);
	BOOST_TEST(*tc->children(*child2).at(0)->version() == 201);
	BOOST_CHECK(!tc->node_exists(child12.get()));
	BOOST_CHECK(!tc->node_exists(child121.get()));
}
BOOST_FIXTURE_TEST_CASE(getters, trees_fixture)
{
	mk_node(t1(), t1().root(), {});
	auto tc_1 = t1().copy_if([](auto &) { return true; });
	BOOST_CHECK( tc_1.has_value()) ;
	BOOST_TEST(tc_1.value().data_id()==t1().data_id());
	BOOST_TEST(tc_1.value().root_version()==t1().root_version());

	t1().root_version(10);
	BOOST_TEST(tc_1.value().root_version()!=t1().root_version());
	auto tc_2 = t1().copy_if([](auto &) { return true; });
	BOOST_CHECK( tc_2.has_value()) ;
	BOOST_TEST(tc_2.value().root_version() == t1().root_version());
}
BOOST_FIXTURE_TEST_CASE(uncond_parent_not_copying_children, trees_fixture)
{
	auto nodes = mk_tree(t1(), {
	 /*0*/   {std::nullopt, {.node_var=variable{"child1", "1"}}}
	 /*1*/ , {std::nullopt, {.node_var=variable{"child2", "2"}}}
	 /*2*/ , { 0, {.node_var=variable{"child11", "11"}}}
	 /*3*/ , {-1, {.node_var=variable{"child111", "111"}}}
	 /*4*/ , { 0, {.node_var=variable{"child12", "12"}}}
	 /*5*/ , { 1, {.node_var=variable{"child21", "21"}}}
	 /*6*/ , {-1, {.node_var=variable{"child211", "211"}}}
	                     });
	auto child2 = nodes[1];
	auto child11 = nodes[2];

	auto cp = t1().copy_if([child2, child11](auto &node) {
		return &node != child2.get() && &node != child11.get();
	});
	BOOST_CHECK( cp.has_value()) ;
	auto names = cp.value().var_name_list() ;

	BOOST_TEST( names.size() == 2) ;
	for( auto name :names) BOOST_CHECK( name != "child2"sv && name != "child11"sv);
}
BOOST_AUTO_TEST_SUITE_END() // copy
BOOST_FIXTURE_TEST_CASE(node_variables, trees_fixture)
{
	mk_tree(t1(), {
	            {std::nullopt, {.node_var=variable{"vn1", "vv1"}}}
	          , {std::nullopt, {.node_var=variable{"vn2", "vv2"}}}
	          , { 0, {.node_var=variable{"vn3", "vv3_1"}}}
	          , { 1, {}}
	          , { 1, {.node_var=variable{"vn3", "vv3_2"}}}
	        });

	BOOST_TEST_CONTEXT("var name list")
	check_vec_eq(t1().var_name_list(), {"vn1"sv, "vn2"sv, "vn3"sv});
	BOOST_TEST_CONTEXT("var values list for vn1")
	check_vec_eq(t1().var_value_list("vn1"), {"vv1"sv});
	BOOST_TEST_CONTEXT("var values list for vn2")
	check_vec_eq(t1().var_value_list("vn2"), {"vv2"sv});
	BOOST_TEST_CONTEXT("var values list for vn3")
	check_vec_eq(t1().var_value_list("vn3"), {"vv3_1"sv, "vv3_2"sv});
}
BOOST_FIXTURE_TEST_CASE(contains, trees_fixture)
{
	auto nodes = mk_tree(t1(), {
	            {std::nullopt, {.version = 200}},
	            {-1, {}}
	        });
	auto c1 = nodes[0];

	gen_utils::tree other_t1(t1_root, t1_dsl);
	BOOST_CHECK(t1().contains(other_t1) == gen_utils::tree_compare_result::only_root);
	other_t1.add(other_t1.root(), c1);
	BOOST_CHECK(t1().contains(other_t1) == gen_utils::tree_compare_result::total);
	BOOST_CHECK(other_t1.contains(t1()) == gen_utils::tree_compare_result::partial);
	other_t1.add(*c1, nodes[1]);
	BOOST_CHECK(t1().contains(other_t1) == gen_utils::tree_compare_result::total);
	BOOST_CHECK(t1().contains(other_t1) == gen_utils::tree_compare_result::total);

	gen_utils::tree t3(c1, t1_dsl);
	gen_utils::tree t3_eq(c1, t1_dsl);
	BOOST_CHECK(t1().contains(t3) == gen_utils::tree_compare_result::none);
	BOOST_CHECK(t3.contains(t3) == gen_utils::tree_compare_result::total);
	BOOST_CHECK(t3.contains(t3_eq) == gen_utils::tree_compare_result::total);

	auto t4dm = std::make_shared<gen_utils_mocks::dsl_manager>();
	MOCK_EXPECT(t4dm->id).returns("tt");
	gen_utils::tree t4(c1, t4dm);
	BOOST_CHECK(t1().contains(t4) == gen_utils::tree_compare_result::not_comparable);

	t1().add(t1().root(), mk_node({}));
	BOOST_CHECK(t1().contains(other_t1) == gen_utils::tree_compare_result::total);
	BOOST_CHECK(other_t1.contains(t1()) == gen_utils::tree_compare_result::partial);
}
BOOST_FIXTURE_TEST_CASE(contains_with_diff_order, trees_fixture)
{
	auto nodes = mk_tree(t1(), {
	            {std::nullopt, {.version = 200}},
	            {std::nullopt, {.version = 300}},
	            {0, {}}
	        });
	gen_utils::tree other_t1(t1_root, t1_dsl);
	other_t1.add(other_t1.root(), nodes[1], nodes[0]);

	auto c301 = mk_node({.version=301});
	t1().add(t1().root(), c301);
	other_t1.add(other_t1.root(), c301);

	BOOST_CHECK(t1().contains(other_t1) == gen_utils::tree_compare_result::total);
	BOOST_CHECK(other_t1.contains(t1()) == gen_utils::tree_compare_result::partial);

	other_t1.add(*nodes[0], nodes[2]);
	BOOST_CHECK(t1().contains(other_t1) == gen_utils::tree_compare_result::total);
	BOOST_CHECK(other_t1.contains(t1()) == gen_utils::tree_compare_result::total);
}
BOOST_FIXTURE_TEST_CASE(contains_two_child, trees_fixture)
{
	t1().add(t1().root(), mk_node({.version=101}));
	auto c2 = mk_node(t1(), t1().root(), {.version=102});
	gen_utils::tree t2(t1_root, t1_dsl);
	t2.add(t2.root(), c2);
	BOOST_CHECK(t1().contains(t2) == gen_utils::tree_compare_result::total);
	BOOST_CHECK(t2.contains(t1()) == gen_utils::tree_compare_result::partial);

	t1().add(t1().root(), mk_node({.version=103}));
	BOOST_CHECK(t1().contains(t2) == gen_utils::tree_compare_result::total);
	BOOST_CHECK(t2.contains(t1()) == gen_utils::tree_compare_result::partial);
}
BOOST_FIXTURE_TEST_CASE(search, trees_fixture)
{
	t1_root = mk_node({.version=100, .name="m"});
	auto nodes = mk_tree(t1(), {
	            /* 0 */      {std::nullopt, {.name="a"}}
	            /* 1 */    , {std::nullopt, {.name="c3_par"}}
	            /* 2 */    , {std::nullopt, {.name="b"}}
	            /* 3 */    , {-1, {.name="c"}}
	            /* 4 */    , {-2, {.name="c"}}
	            /* 5 */    , { 1, {.name="c"}}
	                     });

	BOOST_TEST(t1().search({}).size() == 0);
	BOOST_TEST(t1().search({"m"}).size() == 0, "root cannot to be found");
	BOOST_TEST(t1().search({"d"}).size() == 0);
	BOOST_TEST(t1().search({"a", "c"}).size() == 0);

	check_vec_eq(t1().search({"c"}), {nodes[3], nodes[4], nodes[5]});
	check_vec_eq(t1().search({"b", "c"}), {nodes[3], nodes[4]});

	BOOST_TEST(t1().search({"a"}).at(0) == nodes[0]);
	BOOST_TEST(t1().search({"b"}).at(0) == nodes[2]);
}
BOOST_FIXTURE_TEST_CASE(merge, trees_fixture)
{
	auto c1 = mk_node(t1(), t1().root(), {.version=101});
	auto c2 = mk_node({.version=102});
	auto c3 = mk_node({.version=103});
	auto c4 = mk_node({.version=104});

	gen_utils::tree t1_c = *t1().copy_if([](auto&){ return true; });
	t1_c.add(t1_c.root(), c2).add(*c2, c3).add(*c3, c4);

	t1().merge(t1_c);
	BOOST_REQUIRE(t1().children(t1().root()).size() == 2);
	BOOST_TEST(t1().children(t1().root())[0].get() == c1.get());
	BOOST_TEST(t1().children(t1().root())[1].get() == c2.get());
	BOOST_TEST(t1().children(*c2).size() == 1);
	BOOST_TEST(t1().children(*c2).at(0) == c3);
	BOOST_TEST(t1().children(*c3).size() == 1);
	BOOST_TEST(t1().children(*c3).at(0) == c4);

	BOOST_CHECK_THROW(t1().merge(t2()), std::exception);
}
BOOST_FIXTURE_TEST_CASE(parent, trees_fixture)
{
	auto c1 = mk_node({.version=110});
	auto c2 = mk_node({.version=120});
	BOOST_CHECK_THROW(t1().parent(*c1), std::exception);
	t1().add(t1().root(), c1, c2);

	auto c12 = mk_node(t1(), *c1, {.version=111});
	auto c13 = mk_node(t1(), *c1, {});

	BOOST_TEST(t1().parent(t1().root()) == nullptr);
	BOOST_TEST(t1().parent(*c2) == &t1().root());
	BOOST_TEST(t1().parent(*c12) == c1.get());
	BOOST_TEST(t1().parent(*c12, [&c1](const auto& n){return &n!=c1.get();}) == &t1().root());
	BOOST_TEST(t1().parent(*c13, [&c1](const auto& n){return &n!=c1.get();}) == &t1().root());
	BOOST_TEST(t1().parent(*c13, [](const auto& n){return false;}) == nullptr);
}
BOOST_AUTO_TEST_SUITE_END() // tree

BOOST_AUTO_TEST_SUITE_END() // input
