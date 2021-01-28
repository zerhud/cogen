/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE imports_manager

#include <boost/test/included/unit_test.hpp>

#include "mocks.hpp"
#include "input/imports_manager.hpp"
#include "input/input.hpp"

using namespace std::literals;
using gunc = gen_utils::name_conversion;

std::pmr::string operator "" _s (const char* d, std::size_t l)
{
	return std::pmr::string(d, l);
}

boost::json::value operator "" _bj(const char* d, std::size_t l)
{
	return boost::json::parse(boost::json::string_view(d,l));
}


BOOST_AUTO_TEST_SUITE(input)

BOOST_AUTO_TEST_SUITE(imports_manager_test)
using gen_utils::imports_manager;
using gen_utils_mocks::trees_fixture;
BOOST_FIXTURE_TEST_CASE(self_matched, trees_fixture)
{
	gen_utils::input fdata1, fdata2;
	auto t1_child1 = gen_utils_mocks::make_node(11);
	t1().add(t1().root(), t1_child1);
	fdata1.add(t1());
	fdata2.add(t1());

	fdata1.conf().naming.clear();
	fdata2.conf().naming.emplace_back(gunc::camel_case);

	imports_manager mng1;
	mng1.add("a", fdata1).add("b", fdata2).build();
	auto r1 = mng1.self_matched(fdata1);
	auto r2 = mng1.self_matched(fdata2);
	BOOST_TEST_REQUIRE(r1.size()==1);
	BOOST_TEST_REQUIRE(r2.size()==1);
	BOOST_TEST(r1.at(0) == "b");
	BOOST_TEST(r2.at(0) == "a");

	auto t1_child2 = gen_utils_mocks::make_node(12);
	t2().add(t2().root(), t1_child1);
	t2().add(t2().root(), t1_child2);
	auto sep_t1 = t2().copy_if(
	            [this, &t1_child2](const gen_utils::data_node& n){
		return &n == t2_root.get() || &n == t1_child2.get(); });
	auto sep_t2 = t2().copy_if(
	            [this, &t1_child1](const gen_utils::data_node& n){
		return &n == t2_root.get() || &n == t1_child1.get(); });
	gen_utils::input fdata3, fdata4, fdata5;
	fdata3.add(sep_t1.value());
	fdata4.add(sep_t2.value());
	fdata5.add(t2());
	imports_manager mng2;
	mng2("a", fdata5)("b", fdata3)("c", fdata4).build();
	auto r3 = mng2.self_matched(fdata5);
	BOOST_TEST_REQUIRE(r3.size()==2);
	BOOST_TEST(r3.at(0) == "b");
	BOOST_TEST(r3.at(1) == "c");
}
BOOST_FIXTURE_TEST_CASE(required_for, trees_fixture)
{
	gen_utils::input fdata1, fdata2;
	auto t1_child1 = gen_utils_mocks::make_node(
	            11, std::nullopt, std::nullopt, "a");
	auto t1_child2 = gen_utils_mocks::make_node(
	            12, std::nullopt, std::nullopt, "b");
	auto t2_child1 = gen_utils_mocks::make_node(
	            12, std::nullopt, std::nullopt, "not_used", {{"a"}, {"b"}});
	auto t2_child2 = gen_utils_mocks::make_node(
	            12, std::nullopt, std::nullopt, "b");
	t1().add(t1().root(), t1_child1);
	t1().add(t1().root(), t1_child2);
	t2().add(t2().root(), t2_child1);
	t2().add(t2().root(), t2_child2);
	fdata1.add(t1());
	fdata2.add(t2());
	fdata1.conf().naming.clear();

	MOCK_EXPECT(t1_child1->link_condition).returns("cond_child1"sv);
	MOCK_EXPECT(t1_child2->link_condition).returns("cond_child2"sv);
	MOCK_EXPECT(t2_child2->link_condition).returns("cond_child2_t2"sv);
	MOCK_EXPECT(t1_child1->imports_modification).returns(std::nullopt);
	MOCK_EXPECT(t1_child2->imports_modification)
	        .returns(gen_utils::import_file{true, "sysfile"});
	MOCK_EXPECT(t2_child2->imports_modification).returns(std::nullopt);

	imports_manager mng1;
	mng1("f1", fdata1)("f2", fdata2).build();

	BOOST_TEST(mng1.required_for(fdata1).size() == 0);

	auto r2 = mng1.required_for(fdata2);
	auto r2_ = mng1.required_for(fdata2);
	r2_.pop_back();
	gen_utils_mocks::check_vec_eq(r2_, mng1.required_for_incs(fdata2));

	BOOST_TEST_REQUIRE(r2.size() == 3);
	BOOST_TEST(r2[0].from.node == t2_child1);
	BOOST_TEST(r2[0].from.owner->data_id() == "t2_id");
	BOOST_TEST(r2[0].to.node == t1_child1);
	BOOST_TEST(r2[0].to.owner->data_id() == "t1_id");
	BOOST_TEST(r2[0].file.sys == false);
	BOOST_TEST(r2[0].file.name == "f1");
	BOOST_TEST(r2[0].cond == "cond_child1");
	BOOST_TEST(r2[0].cfg.naming.size() == 0);

	BOOST_TEST(r2[1].from.node == t2_child1);
	BOOST_TEST(r2[1].from.owner->data_id() == "t2_id");
	BOOST_TEST(r2[1].to.node == t1_child2);
	BOOST_TEST(r2[1].to.owner->data_id() == "t1_id");
	BOOST_TEST(r2[1].file.sys == true);
	BOOST_TEST(r2[1].file.name == "sysfile");
	BOOST_TEST(r2[1].cond == "cond_child2");
	BOOST_TEST(r2[1].cfg.naming.size() == 0);

	BOOST_TEST(r2[2].file.sys == false);
	BOOST_TEST(r2[2].file.name == "f2");

	auto r3 = mng1.required_for(t2());
	BOOST_TEST(r3.size() == r2.size());

	BOOST_TEST(r3[0].from.node == t2_child1);
	BOOST_TEST(r3[0].from.owner->data_id() == "t2_id");
	BOOST_TEST(r3[0].to.node == t1_child1);
	BOOST_TEST(r3[0].to.owner->data_id() == "t1_id");
	BOOST_TEST(r3[0].file.sys == false);
	BOOST_TEST(r3[0].file.name == "f1");
	BOOST_TEST(r3[0].cond == "cond_child1");
	BOOST_TEST(r3[0].cfg.naming.size() == 0);

	BOOST_TEST(r3[1].from.node == t2_child1);
	BOOST_TEST(r3[1].from.owner->data_id() == "t2_id");
	BOOST_TEST(r3[1].to.node == t1_child2);
	BOOST_TEST(r3[1].to.owner->data_id() == "t1_id");
	BOOST_TEST(r3[1].file.sys == true);
	BOOST_TEST(r3[1].file.name == "sysfile");
	BOOST_TEST(r3[1].cond == "cond_child2");
	BOOST_TEST(r3[1].cfg.naming.size() == 0);
}
BOOST_FIXTURE_TEST_CASE(all_includes, trees_fixture)
{
	gen_utils::input fdata1, fdata2;
	auto t1_child1 = gen_utils_mocks::make_node(
	            11, std::nullopt, std::nullopt, "a");
	auto t1_child2 = gen_utils_mocks::make_node(
	            12, std::nullopt, std::nullopt, "b");
	auto t2_child1 = gen_utils_mocks::make_node(
	            12, std::nullopt, std::nullopt, "not_used", {{"a"}, {"b"}});
	auto t2_child2 = gen_utils_mocks::make_node(
	            12, std::nullopt, std::nullopt, "b");
	t1().add(t1().root(), t1_child1);
	t1().add(t1().root(), t1_child2);
	t2().add(t2().root(), t2_child1);
	t2().add(t2().root(), t2_child2);
	fdata1.add(t1());
	fdata2.add(t2());
	fdata1.conf().naming.clear();

	MOCK_EXPECT(t1_child1->link_condition).returns("cond_child1"sv);
	MOCK_EXPECT(t1_child2->link_condition).returns("cond_child2"sv);
	MOCK_EXPECT(t2_child2->link_condition).returns("cond_child2_t2"sv);
	MOCK_EXPECT(t1_child1->imports_modification).returns(std::nullopt);
	MOCK_EXPECT(t1_child2->imports_modification)
	        .returns(gen_utils::import_file{true, "sysfile"});
	MOCK_EXPECT(t2_child2->imports_modification).returns(std::nullopt);

	imports_manager mng1;
	mng1("f1", fdata1)("f2", fdata2).build();

	auto incs = mng1.all_includes(fdata2);
	BOOST_TEST(incs.size() == 2);
	BOOST_TEST(incs["cond_child1"].size() == 1);
	BOOST_TEST(incs["cond_child2"].size() == 1);

	BOOST_TEST(incs["cond_child1"].at(0).sys == false);
	BOOST_TEST(incs["cond_child1"].at(0).name == "f1");

	BOOST_TEST(incs["cond_child2"].at(0).sys == true);
	BOOST_TEST(incs["cond_child2"].at(0).name == "sysfile");

}
BOOST_AUTO_TEST_SUITE_END() // imports_manager_test

BOOST_AUTO_TEST_SUITE_END() // input
