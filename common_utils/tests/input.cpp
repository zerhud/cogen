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

#include "common_utils/input/tree.hpp"
#include "common_utils/input/map_to.hpp"
#include "common_utils/input/complilation_config.hpp"
#include "mocks.hpp"

using namespace std::literals;

std::pmr::string operator "" _s (const char* d, std::size_t l)
{
	return std::pmr::string(d, l);
}

BOOST_AUTO_TEST_SUITE(input)
struct fixture : gen_utils_mocks::base_tree_fixture {
	std::shared_ptr<gen_utils_mocks::data_node> make_node(
	              std::optional<std::uint64_t> v
	            , std::optional<std::pmr::string> name=std::nullopt
	            , std::optional<std::pmr::string> value=std::nullopt
	            )
	{
		return gen_utils_mocks::make_node(v, std::move(name), std::move(value));
	}

	template<typename T, typename L>
	void check_vec(const std::pmr::vector<T>& vec, std::initializer_list<L> list) const
	{
		BOOST_TEST_REQUIRE(vec.size()==list.size());
		auto contains = [&vec](const auto& v) {
			auto pos = std::find(vec.begin(),vec.end(), v);
			return pos != vec.end();
		};
		for(auto& lv:list) {
			BOOST_TEST_CONTEXT("checking " << lv) {
				BOOST_CHECK(contains(lv));
			}
		}
	}

	template<typename Map, typename Key>
	bool map_contains(const Map& map, const Key& key)
	{
		return map.find(key) != map.end();
	}
};
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

	gen_utils_mocks::compilation_config cfg;
	MOCK_EXPECT(dm->to_json).calls([&t,&cfg](
			const gen_utils::compilation_config& c, const gen_utils::tree& gt){
		BOOST_TEST(&c == &cfg);
		BOOST_CHECK(&gt == &t);
		boost::json::object ret;
		ret["test"] = "ok";
		return ret; });
	BOOST_TEST(t.to_json(cfg) == boost::json::parse(R"({"test":"ok"})"));
}
BOOST_AUTO_TEST_SUITE(add_children)
BOOST_FIXTURE_TEST_CASE(no_parent, fixture)
{
	auto bad_node = std::make_shared<gen_utils_mocks::data_node>();
	auto bad_node2 = std::make_shared<gen_utils_mocks::data_node>();
	BOOST_CHECK_THROW((void)tree().children(*bad_node), std::exception);
	BOOST_CHECK_THROW(tree().add(*bad_node, bad_node2), std::exception);
}
BOOST_FIXTURE_TEST_CASE(cannot_add_low_version, fixture)
{
	auto bad_node = make_node(tree().root_version()-1);
	BOOST_CHECK_THROW(tree().add(tree().root(), bad_node), std::exception);
}
BOOST_FIXTURE_TEST_CASE(adding_twice, fixture)
{
	auto bad_node = make_node(std::nullopt);
	tree().add(tree().root(), bad_node);
	BOOST_CHECK_THROW(tree().add(tree().root(), bad_node), std::exception);
}
BOOST_FIXTURE_TEST_CASE(can_find_child, fixture)
{
	auto child1 = make_node(std::nullopt);
	auto child2 = make_node(std::nullopt);
	auto child12 = make_node(std::nullopt);
	tree().add(tree().root(), child1);
	tree().add(tree().root(), child2);
	tree().add(*child1, child12);
	BOOST_TEST_REQUIRE(tree().children(tree().root()).size()==2);
	BOOST_TEST(tree().children(tree().root())[0]==child1);
	BOOST_TEST(tree().children(tree().root())[1]==child2);
	BOOST_TEST_REQUIRE(tree().children(*child1).size()==1);
	BOOST_TEST(tree().children(*child1)[0]==child12);
}
BOOST_AUTO_TEST_SUITE_END() // add_children
BOOST_FIXTURE_TEST_CASE(next_min_version, fixture)
{
	BOOST_TEST(*main_node->version()==100);
	BOOST_TEST(tree().next_min_version()==100);
	BOOST_TEST(tree().root_version()==100);

	auto child1 = make_node(110);
	tree().add(tree().root(), child1);
	BOOST_TEST(tree().next_min_version()==110);
	BOOST_TEST(tree().root_version()==100);

	auto child2 = make_node(std::nullopt);
	tree().add(tree().root(), child2);
	BOOST_TEST(tree().next_min_version()==110);
	BOOST_TEST(tree().root_version()==100);

	auto child12 = make_node(105);
	tree().add(*child1, child12);
	BOOST_TEST(tree().next_min_version()==105);
	BOOST_TEST(tree().root_version()==100);
}
BOOST_AUTO_TEST_SUITE(copy)
BOOST_FIXTURE_TEST_CASE(no_cond, fixture)
{
	auto child1 = make_node(std::nullopt);
	auto child11 = make_node(std::nullopt);
	tree().add(tree().root(), child1);
	tree().add(*child1, child11);

	auto tcopy = tree().copy_if(gen_utils::tree::copy_condition{});
	BOOST_CHECK( !tcopy.has_value()) ;
}
BOOST_FIXTURE_TEST_CASE(empty_tree, fixture)
{
	auto tc = tree().copy_if([](auto &) { return false; });
	BOOST_CHECK( !tc);
}
BOOST_FIXTURE_TEST_CASE(few_childs, fixture)
{
	auto child1 = make_node(std::nullopt);
	auto child2 = make_node(std::nullopt);
	auto child11 = make_node(std::nullopt);
	auto child12 = make_node(std::nullopt);
	tree().add(tree().root(), child1);
	tree().add(tree().root(), child2);
	tree().add(*child1, child11);
	tree().add(*child1, child12);

	auto tc = tree().copy_if([child2, child12](auto &n) {
		return !(&n == child2.get() || &n == child12.get());
	});
	BOOST_CHECK( tc.has_value()) ;
	BOOST_TEST_REQUIRE(tc.value().children(tc->root()).size()==1);
	BOOST_TEST(tc.value().children(tc->root())[0] == child1);
	BOOST_TEST(tc.value().children(*child1).size() == 1);
	BOOST_TEST(tc.value().children(*child1).at(0) == child11);
}
BOOST_FIXTURE_TEST_CASE(getters, fixture)
{
	auto child1 = make_node(std::nullopt);
	tree().add(tree().root(), child1);
	auto tc_1 = tree().copy_if([](auto &) { return true; });
	BOOST_CHECK( tc_1.has_value()) ;
	BOOST_TEST(tc_1.value().data_id()==tree().data_id());
	BOOST_TEST(tc_1.value().root_version()==tree().root_version());

	tree().root_version(10);
	BOOST_TEST(tc_1.value().root_version()!=tree().root_version());
	auto tc_2 = tree().copy_if([](auto &) { return true; });
	BOOST_CHECK( tc_2.has_value()) ;
	BOOST_TEST(tc_2.value().root_version() == tree().root_version());
}

BOOST_FIXTURE_TEST_CASE(uncond_parent_not_copying_children, fixture)
{
	auto child1   = make_node(std::nullopt, "child1", "1");
	auto child11  = make_node(std::nullopt, "child11", "1");
	auto child111 = make_node(std::nullopt, "child111", "1");
	auto child12  = make_node(std::nullopt, "child12", "1");
	auto child2   = make_node(std::nullopt, "child2", "1");
	auto child21  = make_node(std::nullopt, "child21", "1");
	auto child211 = make_node(std::nullopt, "child211", "1");

	tree().add(tree().root(), child1);
	tree().add(tree().root(), child2);
	tree().add(*child1, child11);
	tree().add(*child11, child111);
	tree().add(*child1, child12);
	tree().add(*child2, child21);
	tree().add(*child2, child211);

	auto cp = tree().copy_if([child2, child11](auto &node) {
		return &node != child2.get() && &node != child11.get();
	});
	BOOST_CHECK( cp.has_value()) ;
	auto names = cp.value().var_name_list() ;

	BOOST_TEST( names.size() == 2) ;
	for( auto name :names) BOOST_CHECK( name != "child2"sv && name != "child11"sv);
}

BOOST_AUTO_TEST_SUITE_END() // copy
BOOST_FIXTURE_TEST_CASE(node_variables, fixture)
{
	auto n1 = make_node(std::nullopt, "vn1", "vv1");
	auto n2 = make_node(std::nullopt, "vn2", "vv2");
	auto n3 = make_node(std::nullopt, "vn3", "vv3_1");
	auto n4 = make_node(std::nullopt);
	auto n5 = make_node(std::nullopt, "vn3", "vv3_2");
	tree().add(tree().root(), n1);
	tree().add(tree().root(), n2);
	tree().add(*n1, n3);
	tree().add(*n2, n4);
	tree().add(*n2, n5);

	BOOST_TEST_CONTEXT("var name list")
	check_vec(tree().var_name_list(), {"vn1"sv, "vn2"sv, "vn3"sv});
	BOOST_TEST_CONTEXT("var values list for vn1")
	check_vec(tree().var_value_list("vn1"), {"vv1"sv});
	BOOST_TEST_CONTEXT("var values list for vn2")
	check_vec(tree().var_value_list("vn2"), {"vv2"sv});
	BOOST_TEST_CONTEXT("var values list for vn3")
	check_vec(tree().var_value_list("vn3"), {"vv3_1"sv, "vv3_2"sv});
}
BOOST_FIXTURE_TEST_CASE(contains, fixture)
{
	auto c1 = make_node(200);
	auto c11 = make_node(std::nullopt);
	tree().add(tree().root(), c1);
	tree().add(*c1, c11);
	gen_utils::tree t2(main_node, dmanager);
	BOOST_CHECK(tree().contains(t2) == gen_utils::tree_compare_result::only_root);
	t2.add(t2.root(), c1);
	BOOST_CHECK(tree().contains(t2) == gen_utils::tree_compare_result::partial);
	t2.add(*c1, c11);
	BOOST_CHECK(tree().contains(t2) == gen_utils::tree_compare_result::total);

	gen_utils::tree t3(c1, dmanager);
	BOOST_CHECK(tree().contains(t3) == gen_utils::tree_compare_result::none);

	auto t4dm = std::make_shared<gen_utils_mocks::dsl_manager>();
	MOCK_EXPECT(t4dm->id).returns("tt");
	gen_utils::tree t4(c1, t4dm);
	BOOST_CHECK(tree().contains(t4) == gen_utils::tree_compare_result::not_comparable);
}
BOOST_AUTO_TEST_SUITE_END() // tree
BOOST_AUTO_TEST_SUITE(tree_map_to)
using gen_utils::map_to;
BOOST_FIXTURE_TEST_CASE(one_var, fixture)
{
	map_to mapper;
	main_node = make_node(1, "var", "val");
	auto r = mapper("_${var}_", tree());
	BOOST_TEST(r.size()==1);
	BOOST_CHECK(map_contains(r, "_val_"));
}
BOOST_FIXTURE_TEST_CASE(tmpl_diff_from_var_name, fixture)
{
 	map_to mapper;
	main_node = make_node(1, "var", "val");
	auto r = mapper("_${var1}_", tree());
	BOOST_TEST(r.size()==1);
	BOOST_CHECK(map_contains(r, "_${var1}_")) ;
}
BOOST_FIXTURE_TEST_CASE(_3_node_1_tmpl, fixture)
{
	map_to mapper;
	main_node = make_node(1, "var1", "val1");
	auto node2 = make_node(std::nullopt, "var2", "val2");
	auto node3 = make_node(std::nullopt, "var3", "val3");
	tree().add(tree().root(), node2);
	tree().add(tree().root(), node3);
	auto r = mapper("_${var1}_", tree());
	BOOST_TEST(r.size() == 1) ;
	BOOST_CHECK( map_contains(r,"_val1_")) ;
}
BOOST_FIXTURE_TEST_CASE(few_vars, fixture)
{
	map_to mapper;
	main_node = make_node(1, "var1", "val1");
	auto node_2 = make_node(std::nullopt, "var2", "val2");
	auto node_21 = make_node(std::nullopt, "var2", "val21");
	auto node_3 = make_node(std::nullopt, "var3", "val3");
	auto node_31 = make_node(std::nullopt, "var3", "val31");
	tree().add(tree().root(), node_2);
	tree().add(tree().root(), node_3);
	tree().add(*node_3, node_31);
	tree().add(*node_2, node_21);
	auto r = mapper("_${var1}_${var2}_${var3}_${var1}_", tree());
	BOOST_TEST(r.size()==4);
	BOOST_CHECK(map_contains(r, "_val1_val2_val3_val1_"));
	BOOST_CHECK(map_contains(r, "_val1_val2_val31_val1_"));
	BOOST_CHECK(map_contains(r, "_val1_val21_val3_val1_"));
	BOOST_CHECK(map_contains(r, "_val1_val21_val31_val1_"));
}
BOOST_FIXTURE_TEST_CASE(no_var, fixture)
{
	map_to mapper;
	main_node = make_node(1, "var1", "val1");
	auto r = mapper("_${var1}_${var2}_", tree());
	BOOST_TEST(r.size()==1);
	BOOST_CHECK(map_contains(r, "_val1_${var2}_"));
}
BOOST_FIXTURE_TEST_CASE(no_any_var, fixture)
{
	map_to mapper;
	main_node = make_node(1);
	auto r = mapper("_${var1}_${var2}_", tree());
	BOOST_TEST(r.size()==1);
	BOOST_CHECK(map_contains(r, "_${var1}_${var2}_"));
}
BOOST_FIXTURE_TEST_CASE(no_var_no_ref, fixture)
{
	map_to mapper;
	main_node = make_node(1);
	auto r = mapper("_xxx_", tree());
	BOOST_TEST(r.size()==1);
	BOOST_CHECK(map_contains(r, "_xxx_"));
}
BOOST_FIXTURE_TEST_CASE(double_use, fixture)
{
	map_to mapper;
	main_node = make_node(1, "var", "val");
	auto r = mapper("_xxx_", tree());
	BOOST_TEST(r.size()==1);
	BOOST_CHECK(map_contains(r, "_xxx_"));

	r = mapper("_${var}_", tree());
	BOOST_TEST(r.size()==1);
	BOOST_CHECK(map_contains(r, "_val_"));
}
BOOST_AUTO_TEST_SUITE_END() // tree_map_to

BOOST_AUTO_TEST_SUITE(compilation_config)
using gen_utils::compilation_config_impl::compilation_config;
BOOST_AUTO_TEST_CASE(compliller_name)
{
	auto config = std::make_shared<compilation_config>(gen_utils::compiler::cpp);
	BOOST_CHECK( gen_utils::compiler::cpp == config->compiler_name() );

}
BOOST_AUTO_TEST_SUITE_END() // compilation_config
BOOST_AUTO_TEST_SUITE_END() // input
