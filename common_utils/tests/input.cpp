/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
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
using gen_utils_mocks::mk_tree;
using gen_utils_mocks::trees_fixture;

BOOST_AUTO_TEST_SUITE(input)

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
	i1.add(t1).add(t2);

	BOOST_TEST(i1.all().size()==2);
	BOOST_TEST(i2.all().size()==0);

	i2.add(std::move(i1));
	BOOST_TEST(i2.all().size()==2);
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
BOOST_FIXTURE_TEST_CASE(contains, trees_fixture)
{
	ic_input i;
	i.add(t1()).add(t2());
	BOOST_CHECK(  i.contains(i.select(t1().data_id()).at(0)) );
	BOOST_CHECK(  i.contains(i.select(t2().data_id()).at(0)) );
	BOOST_CHECK( !i.contains(&t2()) );
}
BOOST_FIXTURE_TEST_CASE(moving, trees_fixture)
{
	using tcr = gen_utils::tree_compare_result;
	using nc = gen_utils::name_conversion;
	mk_tree(t1(), {{},{}});
	mk_tree(t2(), {{},{}});
	ic_input from, to;
	from
	        .add(t1())
	        .add(t2())
	        .conf().naming.emplace_back(nc::camel_case);

	to = std::move(from);
	BOOST_TEST_REQUIRE(to.all().size() == 2);
	BOOST_CHECK(to.all()[0]->contains(t1()) == tcr::total);
	BOOST_CHECK(to.all()[1]->contains(t2()) == tcr::total);
	BOOST_CHECK(t1().contains(*to.all()[0]) == tcr::total);
	BOOST_CHECK(t2().contains(*to.all()[1]) == tcr::total);

	BOOST_TEST_REQUIRE(to.conf().naming.size() == 2);
	BOOST_TEST(to_string(to.conf().naming[0]) == to_string(nc::as_is));
	BOOST_TEST(to_string(to.conf().naming[1]) == to_string(nc::camel_case));
}
BOOST_AUTO_TEST_SUITE_END() // input

BOOST_AUTO_TEST_SUITE_END() // input
