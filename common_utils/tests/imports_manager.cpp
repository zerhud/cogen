/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE imports_manager

#include <boost/test/included/unit_test.hpp>

#include "mocks.hpp"
#include "utils/map_to.hpp"
#include "utils/input.hpp"
#include "utils/imports_manager.hpp"

using namespace std::literals;
using gunc = gen_utils::name_conversion;

BOOST_AUTO_TEST_SUITE(input)

BOOST_AUTO_TEST_SUITE(imports_manager_test)
using gen_utils::variable;
using gen_utils::imports_manager;
using gen_utils_mocks::mk_node;
using gen_utils_mocks::mk_tree;
using gen_utils_mocks::trees_fixture;
BOOST_FIXTURE_TEST_CASE(required_for, trees_fixture)
{
	gen_utils::input fdata1, fdata2;
	auto t1_child1 = mk_node(t1(), t1().root(), {.version=11, .name="a",
	                                             .link_cond="cond_child1"});
	auto t1_child2 = mk_node(
	            t1(), t1().root(),
	            {.version=12, .name="b", .link_cond="cond_child2",
	             .import_mods=gen_utils::import_file{true, "sysfile"}});
	auto t2_child1 = mk_tree(t2(), {
	            {std::nullopt, {
	                 .version=12, .name="not_used", .links={{"a"}, {"b"}}}}
	          , {std::nullopt, {
	                 .version=12, .name="b", .link_cond="cond_child2_t2"}}
	         })[0];

	fdata1.add(t1());
	fdata2.add(t2());
	fdata1.conf().naming.clear();

	imports_manager mng1;
	mng1("p1", "f1", fdata1)("p1", "f2", fdata2).build();

	BOOST_TEST(mng1.required_for(fdata1).size() == 0);

	auto r2 = mng1.required_for(fdata2);

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
BOOST_FIXTURE_TEST_CASE(required_includes, trees_fixture)
{
	gen_utils::input fdata1, fdata2;
	mk_tree(t1(), {
	      {std::nullopt, {.version=11, .name="a", .link_cond="cond_child1"}}
	    , {std::nullopt, {.version=12, .name="b", .link_cond="cond_child2",
	             .import_mods=gen_utils::import_file{true, "sysfile"}}}
	        });
	mk_tree(t2(), {
	      {std::nullopt, {.version=12, .name="not_used", .links={{"a"}, {"b"}}}}
	    , {std::nullopt, {.version=12, .name="b", .link_cond="cond_child2_t2"}}
	        });

	fdata1.add(t1()).conf().naming.clear();
	fdata2.add(t2());

	imports_manager mng1;
	mng1("p1", "f1", fdata1)("p2", "f2", fdata2).build();

	auto incs = mng1.required_includes(fdata2);
	BOOST_TEST(incs.size() == 2);
	BOOST_TEST(incs["cond_child1"].size() == 1);
	BOOST_TEST(incs["cond_child2"].size() == 1);

	BOOST_TEST(incs["cond_child1"].at(0).sys == false);
	BOOST_TEST(incs["cond_child1"].at(0).name == "f1");

	BOOST_TEST(incs["cond_child2"].at(0).sys == true);
	BOOST_TEST(incs["cond_child2"].at(0).name == "sysfile");
}
BOOST_FIXTURE_TEST_CASE(required_includes_with_own, trees_fixture)
{
	mk_tree(t1(), {
	      {std::nullopt, {.version=11, .name="a",
	       .node_var=variable{"v", "vv1"}, .link_cond="cond_child1"} }
	    , {std::nullopt, {.version=12, .name="b",
	       .node_var=variable{"v", "vv2"}, .links={{"a"}} } }
	        });

	auto mapped = gen_utils::map_to()("${v}", t1());
	gen_utils::input fdata1, fdata2;
	fdata1.add(mapped.at("vv1"));
	fdata2.add(mapped.at("vv2"));

	imports_manager mng1;
	mng1("p1", "f1", fdata1);
	mng1("p1", "f2", fdata2);

	auto incs = mng1.required_includes_with_own(fdata1);
	BOOST_TEST(incs.size() == 0);

	incs = mng1.required_includes_with_own(fdata2);
	BOOST_TEST(incs.size() == 1);
	BOOST_TEST(incs["cond_child1"].size() == 1);

	BOOST_TEST(incs["cond_child1"].at(0).sys == false);
	BOOST_TEST(incs["cond_child1"].at(0).name == "f1");
}
BOOST_FIXTURE_TEST_CASE(never_include_itself, trees_fixture)
{
	mk_tree(t1(), {
	      {std::nullopt, {.version=11, .name="a", .link_cond="cond_child1"}}
	    , {std::nullopt, {.version=12, .name="b", .links={{"a"}}}},
	        });

	gen_utils::input fdata1;
	fdata1.add(t1());

	imports_manager mng1;
	mng1("p1", "f1", fdata1);

	BOOST_TEST(mng1.required_includes(fdata1).size() == 0);
	BOOST_TEST(mng1.required_includes_with_own(fdata1).size() == 0);
}
BOOST_FIXTURE_TEST_CASE(mapped_includes, trees_fixture)
{
	mk_tree(t1(), {
	            {std::nullopt, {.version=1, .node_var=variable{"v1", "m1"}}}
	          , {std::nullopt, {.version=1, .node_var=variable{"v1", "m2"}}}
	        });
	mk_tree(t2(), {
	            {std::nullopt, {.version=2, .node_var=variable{"v2", "n1"}}}
	          , {std::nullopt, {.version=2}}
	        });

	gen_utils::input all_data;
	all_data.add(t1()).add(t2());

	imports_manager mng;
	auto mapped = gen_utils::map_to()("m_${v1}", all_data);
	for(auto& [n,d]:mapped) mng("p1", n,d);
	mng.build();

	BOOST_TEST(mng.mapped_includes("${v1}", mapped["m_m1"]).size()==0);

	auto mr = mng.mapped_includes("f_${v1}", all_data);
	BOOST_TEST(mr.size() == 2);
	BOOST_TEST(mr.at("f_m1").size() == 1);
	gen_utils_mocks::check_vec_eq(mr.at("f_m1"),
	    {gen_utils::import_file{false,"m_m1"_s}});
	BOOST_TEST(mr.at("f_m2").size() == 1);
	gen_utils_mocks::check_vec_eq(mr.at("f_m2"),
	    {gen_utils::import_file{false,"m_m2"_s}});

	mr = mng.mapped_includes("f_${v1}", gen_utils::input().add(t1()));
	BOOST_TEST(mr.size() == 2);
	BOOST_TEST(mr.at("f_m1").size() == 1);
	gen_utils_mocks::check_vec_eq(mr.at("f_m1"),
	    {gen_utils::import_file{false,"m_m1"_s}});
	BOOST_TEST(mr.at("f_m2").size() == 1);
	gen_utils_mocks::check_vec_eq(mr.at("f_m2"),
	    {gen_utils::import_file{false,"m_m2"_s}});

	mr = mng.mapped_includes("f_${v2}", gen_utils::input().add(t2()));
	BOOST_TEST(mr.size() == 1);
	gen_utils_mocks::check_vec_eq(mr.at("f_n1"),
	    {gen_utils::import_file{false,"m_m1"_s},
	    gen_utils::import_file{false,"m_m2"_s}});
}
BOOST_FIXTURE_TEST_CASE(exclude_own_part, trees_fixture)
{
	gen_utils::input fdata1, fdata2;
	mk_tree(t1(), {
	      {std::nullopt, {.version=11, .name="a", .link_cond="cond_child1"}}
	    , {std::nullopt, {.version=12, .name="b", .link_cond="cond_child2",
	             .import_mods=gen_utils::import_file{true, "sysfile"}}}
	        });
	mk_tree(t2(), {
	      {std::nullopt, {.version=12, .name="not_used", .links={{"a"}, {"b"}}}}
	    , {std::nullopt, {.version=12, .name="b", .link_cond="cond_child2_t2"}}
	        });

	fdata1.add(t1()).conf().naming.clear();
	fdata2.add(t2());

	imports_manager mng1;
	mng1("p1", "f1", fdata1)("p1", "f2", fdata2).build();
	BOOST_TEST(mng1.required_for(fdata2).size() == 3);
	BOOST_TEST(mng1.required_includes(fdata2).size() == 1);
	BOOST_TEST(mng1.required_includes(fdata2).at("cond_child2").front().name == "sysfile");
}
BOOST_AUTO_TEST_SUITE_END() // imports_manager_test

BOOST_AUTO_TEST_SUITE_END() // input
