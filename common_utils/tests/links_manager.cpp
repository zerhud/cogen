/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE links_manager

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include "common_utils/input/links.hpp"
#include "common_utils/input/compiled_result.hpp"
#include "mocks.hpp"

using namespace std::literals;

std::pmr::string operator "" _s (const char* d, std::size_t l)
{
	return std::pmr::string(d, l);
}

BOOST_AUTO_TEST_SUITE(input)
struct fixture : gen_utils_mocks::base_tree_fixture
{
	std::shared_ptr<gen_utils_mocks::data_node> other_node;
	std::shared_ptr<gen_utils_mocks::dsl_manager> other_dmng =
			std::make_shared<gen_utils_mocks::dsl_manager>();
	std::optional<gen_utils::tree> _other_tree;

	gen_utils::tree& other_tree() {
		if(!_other_tree)
			_other_tree.emplace(other_node, other_dmng);
		return *_other_tree;
	}

	fixture()
	{
		other_node = gen_utils_mocks::make_node(2);
	}
};
BOOST_AUTO_TEST_SUITE(links_manager)
using gen_utils::links_manager;
using gen_utils::name_t;
using gen_utils_mocks::make_node;
BOOST_FIXTURE_TEST_CASE(top_level, fixture)
{
	main_node = make_node(
	            1, std::nullopt, std::nullopt,
	            "main_node", {{"a"_s}});
	MOCK_EXPECT(other_dmng->id).returns("other"sv);
	MOCK_EXPECT(other_node->name).returns("a"sv);

	links_manager mng({&tree(),&other_tree()});
	BOOST_TEST(mng.links(*main_node).size()==1);
}
BOOST_FIXTURE_TEST_CASE(search_in_children, fixture)
{
	auto main_child = make_node(11, std::nullopt, std::nullopt, "b");
	auto main_child2 = make_node(12, std::nullopt, std::nullopt, "not_in_search");
	main_node = make_node(
	            1, std::nullopt, std::nullopt,
	            "main_node", {{"o"_s,"a"_s},{"data_id"_s,"b"_s}});
	tree().add(*main_node, main_child);
	tree().add(*main_node, main_child2);

	auto other_child = make_node(22, std::nullopt, std::nullopt, "a");
	MOCK_EXPECT(other_dmng->id).returns("o"sv);
	MOCK_EXPECT(other_node->name).returns("other_root"sv);

	other_tree().add(other_tree().root(), other_child);

	links_manager mng({&tree(),&other_tree()});
	BOOST_TEST(mng.links(*main_node).size()==2);
	for(auto& l:mng.links(*main_node)) {
		BOOST_REQUIRE(l.node != nullptr);
		if(l.dsl_id=="data_id")
			BOOST_TEST(l.node == main_child.get());
		else if(l.dsl_id=="o")
			BOOST_TEST(l.node == other_child.get());
		else BOOST_FAIL("unknown node in links: " << l.node->name());
	}
}
BOOST_FIXTURE_TEST_CASE(not_found, fixture)
{
	main_node = make_node(
	            1, std::nullopt, std::nullopt,
	            "main_node", {{"o"_s,"a"_s},{"data_id"_s,"b"_s}});
	links_manager mng({&tree()});
	BOOST_TEST(mng.links(*main_node).size()==0);
}
BOOST_AUTO_TEST_SUITE_END() // links_manager
BOOST_AUTO_TEST_SUITE(compiled_result)
using gen_utils::compiled_result;
BOOST_AUTO_TEST_CASE(data)
{
	compiled_result cr;
	boost::json::object obj;
	obj["a"] = "test";
	cr.data(obj);
	BOOST_TEST(cr.data() == obj);
}
BOOST_AUTO_TEST_CASE(imports)
{
	compiled_result cr;
	BOOST_TEST(cr.imports().size() == 0);

	gen_utils::import_info ii1, ii2;
	ii1.name = "ii1";
	ii2.name = "ii2";

	cr.add_import(ii1);
	BOOST_TEST(cr.imports().size() == 1);
	BOOST_TEST(cr.imports().at(0).name == "ii1");

	cr.add_import(ii2);
	BOOST_TEST(cr.imports().size() == 2);
	BOOST_TEST(cr.imports().at(1).name == "ii2");
}
BOOST_AUTO_TEST_CASE(combine)
{
	compiled_result cr1, cr2, cr3;
	gen_utils::node_pointer n1{gen_utils_mocks::make_node(1), nullptr};
	gen_utils::node_pointer n2{gen_utils_mocks::make_node(2), nullptr};

	gen_utils::import_info
	        ii1{"i", "", n1}, ii2{"i", "", n2};

	cr1.combine(cr2);
	BOOST_TEST(cr1.imports().size() == 0);

	cr2.add_import(ii1);
	cr1.combine(cr2);
	BOOST_TEST(cr1.imports().size() == 1);
	BOOST_TEST(cr1.imports().at(0).name == "i");
	BOOST_CHECK(cr1.imports().at(0).node == n1);

	cr1.combine(cr3);
	BOOST_TEST(cr1.imports().size() == 1);
	BOOST_TEST(cr1.imports().at(0).name == "i");
	BOOST_CHECK(cr1.imports().at(0).node == n1);

	cr1.combine(cr2);
	BOOST_TEST(cr1.imports().size() == 1);
	BOOST_TEST(cr1.imports().at(0).name == "i");
	BOOST_CHECK(cr1.imports().at(0).node == n1);

	cr2.add_import(ii2);
	cr1.combine(cr2);
	BOOST_TEST(cr1.imports().size() == 2);
	BOOST_TEST(cr1.imports().at(0).name == "i");
	BOOST_CHECK(cr1.imports().at(0).node == n1);
	BOOST_TEST(cr1.imports().at(1).name == "i");
	BOOST_CHECK(cr1.imports().at(1).node == n2);

}
BOOST_AUTO_TEST_SUITE_END() // compiled_result
BOOST_AUTO_TEST_SUITE_END() // input
