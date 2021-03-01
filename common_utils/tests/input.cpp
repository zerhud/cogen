/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE utils_input

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "utils/tree.hpp"
#include "utils/input.hpp"
#include "mocks.hpp"

using namespace std::literals;
using gen_utils_mocks::trees_fixture;
using gen_utils_mocks::check_vec_eq;

std::pmr::string operator "" _s (const char* d, std::size_t l)
{
	return std::pmr::string(d, l);
}

BOOST_AUTO_TEST_SUITE(input)
using gen_utils_mocks::make_node;
BOOST_AUTO_TEST_SUITE(tree)
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
	auto bad_node = make_node(t1().root_version()-1);
	BOOST_CHECK_THROW(t1().add(t1().root(), bad_node), std::exception);
}
BOOST_FIXTURE_TEST_CASE(adding_twice, trees_fixture)
{
	auto bad_node = make_node(std::nullopt);
	t1().add(t1().root(), bad_node);
	BOOST_CHECK_THROW(t1().add(t1().root(), bad_node), std::exception);
}
BOOST_FIXTURE_TEST_CASE(can_find_child, trees_fixture)
{
	auto child1 = make_node(std::nullopt);
	auto child2 = make_node(std::nullopt);
	auto child12 = make_node(std::nullopt);
	t1().add(t1().root(), child1);
	t1().add(t1().root(), child2);
	t1().add(*child1, child12);
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

	auto child1 = make_node(110);
	t1().add(t1().root(), child1);
	BOOST_TEST(t1().next_min_version()==110);
	BOOST_TEST(t1().root_version()==1);

	auto child2 = make_node(std::nullopt);
	t1().add(t1().root(), child2);
	BOOST_TEST(t1().next_min_version()==110);
	BOOST_TEST(t1().root_version()==1);

	auto child12 = make_node(105);
	t1().add(*child1, child12);
	BOOST_TEST(t1().next_min_version()==105);
	BOOST_TEST(t1().root_version()==1);

	t1().root_version(106);
	BOOST_TEST(t1().next_min_version()==110);
}
BOOST_AUTO_TEST_SUITE(copy)
BOOST_FIXTURE_TEST_CASE(no_cond, trees_fixture)
{
	auto child1 = make_node(std::nullopt);
	auto child11 = make_node(std::nullopt);
	t1().add(t1().root(), child1);
	t1().add(*child1, child11);

	auto tcopy = t1().copy_if(gen_utils::tree::copy_condition{});
	BOOST_CHECK( !tcopy.has_value()) ;
}
BOOST_FIXTURE_TEST_CASE(empty_tree, trees_fixture)
{
	auto tc = t1().copy_if([](auto &) { return false; });
	BOOST_CHECK( !tc);
}
BOOST_FIXTURE_TEST_CASE(few_childs, trees_fixture)
{
	auto child1 = make_node(std::nullopt);
	auto child2 = make_node(std::nullopt);
	auto child11 = make_node(std::nullopt);
	auto child12 = make_node(std::nullopt);
	t1().add(t1().root(), child1);
	t1().add(t1().root(), child2);
	t1().add(*child1, child11);
	t1().add(*child1, child12);

	auto tc = t1().copy_if([child2, child12](auto& n) {
		return !(&n == child2.get() || &n == child12.get());
	});
	BOOST_REQUIRE( tc.has_value()) ;
	BOOST_TEST_REQUIRE(tc->children(tc->root()).size()==1);
	BOOST_TEST(tc->children(tc->root())[0] == child1);
	BOOST_TEST(tc->children(*child1).size() == 1);
	BOOST_TEST(tc->children(*child1).at(0) == child11);

	auto child121 = make_node(111);
	t1().add(*child2, make_node(201));
	t1().add(*child12, child121);

	tc = t1().copy_if([child1](auto& n){ return &n != child1.get(); });
	BOOST_REQUIRE(tc.has_value());
	BOOST_TEST_REQUIRE(tc->children(tc->root()).size()==1);
	BOOST_TEST(tc->children(tc->root())[0] == child2);
	BOOST_CHECK_THROW(tc->children(*child1).size(), std::exception);
	BOOST_TEST(tc->children(*child2).size() == 1);
	BOOST_TEST(*tc->children(*child2).at(0)->version() == 201);
	BOOST_CHECK(!tc->node_exists(child12.get()));
	BOOST_CHECK(!tc->node_exists(child121.get()));
}
BOOST_FIXTURE_TEST_CASE(getters, trees_fixture)
{
	auto child1 = make_node(std::nullopt);
	t1().add(t1().root(), child1);
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
	auto child1   = make_node(std::nullopt, "child1", "1");
	auto child11  = make_node(std::nullopt, "child11", "1");
	auto child111 = make_node(std::nullopt, "child111", "1");
	auto child12  = make_node(std::nullopt, "child12", "1");
	auto child2   = make_node(std::nullopt, "child2", "1");
	auto child21  = make_node(std::nullopt, "child21", "1");
	auto child211 = make_node(std::nullopt, "child211", "1");

	t1().add(t1().root(), child1);
	t1().add(t1().root(), child2);
	t1().add(*child1, child11);
	t1().add(*child11, child111);
	t1().add(*child1, child12);
	t1().add(*child2, child21);
	t1().add(*child2, child211);

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
	auto n1 = make_node(std::nullopt, "vn1", "vv1");
	auto n2 = make_node(std::nullopt, "vn2", "vv2");
	auto n3 = make_node(std::nullopt, "vn3", "vv3_1");
	auto n4 = make_node(std::nullopt);
	auto n5 = make_node(std::nullopt, "vn3", "vv3_2");
	t1().add(t1().root(), n1);
	t1().add(t1().root(), n2);
	t1().add(*n1, n3);
	t1().add(*n2, n4);
	t1().add(*n2, n5);

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
	auto c1 = make_node(200);
	auto c11 = make_node(std::nullopt);
	t1().add(t1().root(), c1);
	t1().add(*c1, c11);
	gen_utils::tree other_t1(t1_root, t1_dsl);
	BOOST_CHECK(t1().contains(other_t1) == gen_utils::tree_compare_result::only_root);
	other_t1.add(other_t1.root(), c1);
	BOOST_CHECK(t1().contains(other_t1) == gen_utils::tree_compare_result::partial);
	other_t1.add(*c1, c11);
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
}
BOOST_FIXTURE_TEST_CASE(contains_two_child, trees_fixture)
{
	auto c1 = make_node(101);
	auto c2 = make_node(102);
	t1().add(t1().root(), c1);
	t1().add(t1().root(), c2);
	gen_utils::tree t2(t1_root, t1_dsl);
	t2.add(t2.root(), c2);
	BOOST_CHECK(t1().contains(t2) == gen_utils::tree_compare_result::partial);

	t1().add(t1().root(), make_node(103));
	BOOST_CHECK(t1().contains(t2) == gen_utils::tree_compare_result::partial);
}
BOOST_FIXTURE_TEST_CASE(search, trees_fixture)
{
	t1_root = make_node(100, std::nullopt, std::nullopt, "m");
	auto node_b = make_node(120, std::nullopt, std::nullopt, "b");
	auto node_a = make_node(110, std::nullopt, std::nullopt, "a");
	auto node_c = make_node(121, std::nullopt, std::nullopt, "c");
	auto node_c2 = make_node(121, std::nullopt, std::nullopt, "c");
	auto node_c3 = make_node(131, std::nullopt, std::nullopt, "c");
	auto node_c3_par = make_node(130, std::nullopt, std::nullopt, "c3_par");
	t1().add(t1().root(), node_a);
	t1().add(t1().root(), node_b);
	t1().add(t1().root(), node_c3_par);
	t1().add(*node_b, node_c);
	t1().add(*node_b, node_c2);
	t1().add(*node_c3_par, node_c3);

	BOOST_TEST(t1().search({}).size() == 0);
	BOOST_TEST(t1().search({"m"}).size() == 0, "root cannot to be found");
	BOOST_TEST(t1().search({"d"}).size() == 0);
	BOOST_TEST(t1().search({"a", "c"}).size() == 0);

	BOOST_TEST(t1().search({"a"}).at(0) == node_a);
	BOOST_TEST(t1().search({"b"}).at(0) == node_b);
	BOOST_TEST(t1().search({"c"}).at(0) == node_c);
	BOOST_TEST(t1().search({"c"}).at(1) == node_c2);
	BOOST_TEST(t1().search({"c"}).at(2) == node_c3);
	BOOST_TEST(t1().search({"b", "c"}).at(0) == node_c);
	BOOST_TEST(t1().search({"b", "c"}).at(1) == node_c2);
}
BOOST_FIXTURE_TEST_CASE(merge, trees_fixture)
{
	auto c1 = make_node(101);
	auto c2 = make_node(102);
	auto c3 = make_node(103);
	auto c4 = make_node(104);
	t1().add(t1().root(), c1);
	
	gen_utils::tree t1_c =
		*t1().copy_if([](auto& c){ return true; });
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
	auto c1 = make_node(110);
	auto c2 = make_node(120);
	BOOST_CHECK_THROW(t1().parent(*c1), std::exception);
	t1().add(t1().root(), c1);
	t1().add(t1().root(), c2);

	auto c12 = make_node(111);
	auto c13 = make_node(std::nullopt);
	t1().add(*c1, c12);
	t1().add(*c1, c13);

	BOOST_TEST(t1().parent(t1().root()) == nullptr);
	BOOST_TEST(t1().parent(*c2) == &t1().root());
	BOOST_TEST(t1().parent(*c12) == c1.get());
	BOOST_TEST(t1().parent(*c12, [&c1](const auto& n){return &n!=c1.get();}) == &t1().root());
	BOOST_TEST(t1().parent(*c13, [&c1](const auto& n){return &n!=c1.get();}) == &t1().root());
	BOOST_TEST(t1().parent(*c13, [](const auto& n){return false;}) == nullptr);
}
BOOST_AUTO_TEST_SUITE_END() // tree
BOOST_AUTO_TEST_SUITE(input)
using ic_input = gen_utils::input;
BOOST_AUTO_TEST_CASE(getters)
{
	using gunc = gen_utils::name_conversion;
	ic_input i;
	BOOST_TEST(i.all().size() == 0);
	BOOST_TEST(i.select("test"sv).size()==0);
	BOOST_TEST(i.conf().naming.size() == 1);
	BOOST_CHECK(i.conf().naming.at(0) == gunc::as_is);
	i.conf().naming.emplace_back(gunc::camel_case);
	BOOST_CHECK(i.conf().naming.at(0) == gunc::as_is);
	BOOST_CHECK(i.conf().naming.at(1) == gunc::camel_case);
}
BOOST_AUTO_TEST_CASE(adding)
{
	auto m1 = std::make_shared<gen_utils_mocks::dsl_manager>();
	auto m2 = std::make_shared<gen_utils_mocks::dsl_manager>();
	MOCK_EXPECT(m1->id).returns("t1");
	MOCK_EXPECT(m2->id).returns("t2");
	auto n1 = std::make_shared<gen_utils_mocks::data_node>();
	MOCK_EXPECT(n1->version).returns(10);
	gen_utils::tree t1(n1, m1);
	gen_utils::tree t2(n1, m2);
	ic_input i;

	i.add(t1);
	BOOST_TEST_REQUIRE(i.all().size()==1);
	BOOST_TEST(i.all()[0]->data_id()=="t1"sv);
	BOOST_TEST(i.select("t1"sv).at(0)->data_id()=="t1"sv);

	i.add(t2);
	BOOST_TEST_REQUIRE(i.all().size()==2);
	BOOST_TEST(i.select("t1"sv).at(0)->data_id()=="t1"sv);
	BOOST_TEST(i.select("t2"sv).at(0)->data_id()=="t2"sv);

	auto m3 = std::make_shared<gen_utils_mocks::dsl_manager>();
	MOCK_EXPECT(m3->id).returns("t1");
	gen_utils::tree t3(n1, m3);
	i.add(t3);
	BOOST_TEST_REQUIRE(i.all().size()==3);
	BOOST_TEST(i.select("t1"sv).at(0)->data_id()=="t1"sv);
	BOOST_TEST(i.select("t1"sv).at(1)->data_id()=="t1"sv);
}
BOOST_AUTO_TEST_CASE(adding_other)
{
	auto m1 = std::make_shared<gen_utils_mocks::dsl_manager>();
	auto n1 = std::make_shared<gen_utils_mocks::data_node>();
	MOCK_EXPECT(n1->version).returns(10);
	gen_utils::tree t1(n1, m1);
	gen_utils::tree t2(n1, m1);
	gen_utils::tree t3(n1, m1);

	ic_input i1, i2;
	i1.add(t1);
	i1.add(t2);

	BOOST_TEST(i1.all().size()==2);
	BOOST_TEST(i2.all().size()==0);

	i2.add(std::move(i1));
	BOOST_TEST(i2.all().size()==2);
}
BOOST_AUTO_TEST_CASE(contains)
{
	auto m1 = std::make_shared<gen_utils_mocks::dsl_manager>();
	auto m2 = std::make_shared<gen_utils_mocks::dsl_manager>();
	auto m3 = std::make_shared<gen_utils_mocks::dsl_manager>();
	MOCK_EXPECT(m1->id).returns("id1");
	MOCK_EXPECT(m2->id).returns("id2");
	MOCK_EXPECT(m3->id).returns("id3");
	auto t3_r = gen_utils_mocks::make_node(3);
	gen_utils::tree t1(gen_utils_mocks::make_node(1), m1);
	gen_utils::tree t2(gen_utils_mocks::make_node(2), m2);
	gen_utils::tree t3(t3_r, m3);
	gen_utils::tree t4(t3_r, m3);
	t3.add(t3.root(), gen_utils_mocks::make_node(31));

	ic_input i1, i2, i3;
	BOOST_CHECK( i1.match_with(i2) == gen_utils::tree_compare_result::total );

	i1.add(t1);
	i2.add(t3);
	i3.add(t4);

	BOOST_CHECK( i1.match_with(i2) == gen_utils::tree_compare_result::none );

	i1.add(t3); // i1 - t1, t3
	BOOST_CHECK( i1.match_with(i2) == gen_utils::tree_compare_result::total );
	BOOST_CHECK( i2.match_with(i3) == gen_utils::tree_compare_result::only_root );

	i2.add(t1); // t2 - t3, t1
	BOOST_CHECK( i1.match_with(i2) == gen_utils::tree_compare_result::total );
}
BOOST_FIXTURE_TEST_CASE(modify, trees_fixture)
{
	ic_input isrc;
	isrc.conf().naming.clear();
	isrc.add(t1()).add(t2());
	
	ic_input iresult = isrc.modify([this](const gen_utils::tree& src_tree){
			std::pmr::vector<gen_utils::tree> ret;
			ret.emplace_back(src_tree);
			ret.emplace_back(src_tree);
			return ret;
		});

	BOOST_TEST(iresult.conf().naming.size() == 0);
	BOOST_TEST(iresult.all().size() == 4);
	BOOST_TEST(iresult.all().at(0)->data_id() == "t1_id");
	BOOST_TEST(iresult.all().at(1)->data_id() == "t1_id");
	BOOST_TEST(iresult.all().at(2)->data_id() == "t2_id");
	BOOST_TEST(iresult.all().at(3)->data_id() == "t2_id");
}
BOOST_AUTO_TEST_SUITE_END() // input

BOOST_AUTO_TEST_SUITE_END() // input
