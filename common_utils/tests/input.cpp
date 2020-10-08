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
#include "mocks.hpp"

BOOST_AUTO_TEST_SUITE(input)
BOOST_AUTO_TEST_SUITE(tree)
struct fixture {
	std::shared_ptr<gen_utils_mocks::data_node> main_node
	        = std::make_shared<gen_utils_mocks::data_node>();
	std::optional<gen_utils::tree> _tree;
	gen_utils::tree& tree() {
		if(!_tree) _tree.emplace(main_node, 111);
		return *_tree;
	}

	fixture()
	{
		MOCK_EXPECT(main_node->version).returns(100);
	}

	std::shared_ptr<gen_utils_mocks::data_node> make_node(
	            std::optional<std::uint64_t> v)
	{
		auto ret = std::make_shared<gen_utils_mocks::data_node>();
		MOCK_EXPECT(ret->version).returns(v);
		return ret;
	}
};
BOOST_AUTO_TEST_CASE(getters)
{
	BOOST_CHECK_THROW(gen_utils::tree(nullptr, 10), std::exception);

	auto bad_node = std::make_shared<gen_utils_mocks::data_node>();
	MOCK_EXPECT(bad_node->version).returns(std::nullopt);
	BOOST_CHECK_THROW(gen_utils::tree(bad_node, 10), std::exception);

	auto node = std::make_shared<gen_utils_mocks::data_node>();
	MOCK_EXPECT(node->version).returns(1102);
	gen_utils::tree t(node, 10);
	BOOST_TEST(t.data_id() == 10);
	BOOST_TEST(&t.root() == node.get());

	BOOST_TEST(t.root_version() == 1102);
	t.root_version(10);
	BOOST_TEST(t.root_version() == 10);
}
BOOST_AUTO_TEST_SUITE(add_children)
BOOST_FIXTURE_TEST_CASE(no_parent, fixture)
{
	auto bad_node = std::make_shared<gen_utils_mocks::data_node>();
	auto bad_node2 = std::make_shared<gen_utils_mocks::data_node>();
	BOOST_CHECK_THROW(tree().children(*bad_node), std::exception);
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

	auto tcopy = tree().copy(std::function<bool(const gen_utils::data_node&)>{});
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
	auto tc1 = tree().copy( [](auto&){ return true; } );
	BOOST_TEST(tc1.data_id()==tree().data_id());
	BOOST_TEST(tc1.root_version()==tree().root_version());

	tree().root_version(10);
	BOOST_TEST(tc1.root_version()!=tree().root_version());
	auto tc2 = tree().copy( [](auto&){ return true; } );
	BOOST_TEST(tc2.root_version()==tree().root_version());
}
BOOST_AUTO_TEST_SUITE_END() // copy
BOOST_AUTO_TEST_SUITE_END() // tree
BOOST_AUTO_TEST_SUITE_END() // input
