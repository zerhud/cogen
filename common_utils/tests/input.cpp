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
#include "common_utils/input/links.hpp"
#include "mocks.hpp"

using namespace std::literals;

std::pmr::string operator "" _s (const char* d, std::size_t l)
{
	return std::pmr::string(d, l);
}

BOOST_AUTO_TEST_SUITE(input)
struct fixture {
	std::shared_ptr<gen_utils_mocks::dsl_manager> dmanager=
			std::make_shared<gen_utils_mocks::dsl_manager>();
	std::shared_ptr<gen_utils_mocks::data_node> main_node;
	std::optional<gen_utils::tree> _tree;
	gen_utils::tree& tree() {
		if(!_tree) _tree.emplace(main_node, dmanager);
		return *_tree;
	}

	fixture()
	{
		MOCK_EXPECT(dmanager->id).returns("data_id"sv);
		main_node = make_node(100);
	}

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

	auto tcopy = tree().copy(gen_utils::tree::copy_condition{});
	BOOST_TEST(&tcopy.root() == &tree().root());
	BOOST_TEST(tcopy.children(tree().root()).at(0) == child1);
	BOOST_TEST(tcopy.children(*child1).at(0) == child11);
}
BOOST_FIXTURE_TEST_CASE(empty_tree, fixture)
{
	auto tc = tree().copy([](auto&){return false;});
	BOOST_TEST(&tc.root() == &tree().root());
	BOOST_TEST(tc.children(tree().root()).size()==0);
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

	auto tc = tree().copy( [child2, child12](auto& n){
		return !(&n==child2.get() || &n==child12.get());});
	BOOST_TEST_REQUIRE(tc.children(tc.root()).size()==1);
	BOOST_TEST(tc.children(tc.root())[0] == child1);
	BOOST_TEST(tc.children(*child1).size() == 1);
	BOOST_TEST(tc.children(*child1).at(0) == child11);
}
BOOST_FIXTURE_TEST_CASE(getters, fixture)
{
	auto child1 = make_node(std::nullopt);
	tree().add(tree().root(), child1);
	auto tc_1 = tree().copy( [](auto&){ return true; } );
	BOOST_TEST(tc_1.data_id()==tree().data_id());
	BOOST_TEST(tc_1.root_version()==tree().root_version());

	tree().root_version(10);
	BOOST_TEST(tc_1.root_version()!=tree().root_version());
	auto tc_2 = tree().copy([](auto&){ return true; } );
	BOOST_TEST(tc_2.root_version() == tree().root_version());
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

BOOST_AUTO_TEST_SUITE(link_with)
BOOST_AUTO_TEST_SUITE(links_manager)
using gen_utils::links_manager;
using gen_utils::name_t;
BOOST_FIXTURE_TEST_CASE(top_level, fixture)
{
	main_node = make_node(1);
	MOCK_EXPECT(main_node->name).returns("main_node"sv);
	MOCK_EXPECT(main_node->required_links)
	        .returns(std::pmr::vector<name_t>{{"a"_s}});

	auto other_node = make_node(1);
	auto other_dmng = std::make_shared<gen_utils_mocks::dsl_manager>();
	MOCK_EXPECT(other_dmng->id).returns("other"sv);
	MOCK_EXPECT(other_node->name).returns("a"sv);
	MOCK_EXPECT(other_node->required_links).returns(std::pmr::vector<name_t>{});

	MOCK_EXPECT(other_node->name).returns("a"sv);
	gen_utils::tree other_tree(other_node, other_dmng);

	links_manager mng({&tree(),&other_tree});
	BOOST_TEST(mng.links(*main_node).size()==1);
}
BOOST_FIXTURE_TEST_CASE(search_in_children, fixture)
{
	main_node = make_node(1);
	auto main_child = make_node(11);
	auto main_child2 = make_node(12);
	MOCK_EXPECT(main_node->name).returns("main_node"sv);
	MOCK_EXPECT(main_child->name).returns("b"sv);
	MOCK_EXPECT(main_child2->name).returns("not_in_search"sv);
	tree().add(*main_node, main_child);
	tree().add(*main_node, main_child2);
	MOCK_EXPECT(main_node->required_links)
	        .returns(std::pmr::vector<name_t>{{"o"_s,"a"_s},{"data_id"_s,"b"_s}});
	MOCK_EXPECT(main_child->required_links).returns(std::pmr::vector<name_t>{});
	MOCK_EXPECT(main_child2->required_links).returns(std::pmr::vector<name_t>{});

	auto other_root = make_node(2);
	auto other_node = make_node(22);
	auto other_dmng = std::make_shared<gen_utils_mocks::dsl_manager>();
	MOCK_EXPECT(other_dmng->id).returns("o"sv);
	MOCK_EXPECT(other_node->name).returns("a"sv);
	MOCK_EXPECT(other_root->name).returns("other_root"sv);
	MOCK_EXPECT(other_root->required_links).returns(std::pmr::vector<name_t>{});
	MOCK_EXPECT(other_node->required_links).returns(std::pmr::vector<name_t>{});

	MOCK_EXPECT(other_node->name).returns("a"sv);
	gen_utils::tree other_tree(other_root, other_dmng);
	other_tree.add(other_tree.root(), other_node);

	links_manager mng({&tree(),&other_tree});
	BOOST_TEST(mng.links(*main_node).size()==2);
	for(auto& l:mng.links(*main_node)) {
		BOOST_REQUIRE(l.node != nullptr);
		if(l.dsl_id=="data_id")
			BOOST_TEST(l.node == main_child.get());
		else if(l.dsl_id=="o")
			BOOST_TEST(l.node == other_node.get());
		else BOOST_FAIL("unknown node in links: " << l.node->name());
	}
}
BOOST_FIXTURE_TEST_CASE(not_found, fixture)
{
	main_node = make_node(1);
	MOCK_EXPECT(main_node->name).returns("main_node"sv);
	MOCK_EXPECT(main_node->required_links)
	        .returns(std::pmr::vector<name_t>{{"o"_s,"a"_s},{"data_id"_s,"b"_s}});
	links_manager mng({&tree()});
	BOOST_TEST(mng.links(*main_node).size()==0);
}
BOOST_AUTO_TEST_SUITE_END() // links_manager
BOOST_AUTO_TEST_SUITE_END() // link_with

BOOST_AUTO_TEST_SUITE_END() // input
