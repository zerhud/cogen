/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE utils_map

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "utils/tree.hpp"
#include "utils/input.hpp"
#include "utils/map_to.hpp"
#include "utils/map_from.hpp"
#include "mocks.hpp"

using namespace std::literals;
using gen_utils_mocks::check_vec_eq;
using gen_utils_mocks::trees_fixture;

std::ostream& operator << (std::ostream& out, const gen_utils::map_to::result_t& mr)
{
	out << "map " << mr.size() << ": ";
	for(auto& v:mr) out << v.first << ' ';
	return out;
}

std::ostream& operator << (std::ostream& out, const gen_utils::map_from::result_t& vec)
{
	for(auto& [n,d]:vec) {
		out << n << ": (";
		for(auto& v:d) out << v << ' ' ;
		out << ") ";
	}
	return out ;
}

BOOST_AUTO_TEST_SUITE(input)

using gen_utils::map_to;
using gen_utils::map_from;
using gen_utils::variable;
using gen_utils_mocks::mk_node;
using gen_utils_mocks::mk_tree;
using gen_utils_mocks::make_node;
using gen_utils_mocks::trees_fixture;

BOOST_AUTO_TEST_SUITE(tree_map_to)
BOOST_FIXTURE_TEST_CASE(one_var, trees_fixture)
{
	map_to mapper;
	t1_root = mk_node({.version=1, .node_var=variable{"var", "val"}});
	auto r = mapper("_${var}_", t1());
	BOOST_TEST(r.size()==1);
	BOOST_CHECK(r.contains("_val_"));
}
BOOST_FIXTURE_TEST_CASE(tmpl_diff_from_var_name, trees_fixture)
{
	map_to mapper;
	t1_root = mk_node({.version=1, .node_var=variable{"var", "val"}});
	auto r = mapper("_${var1}_", t1());
	BOOST_TEST(r.size()==1);
	BOOST_CHECK(r.contains("_${var1}_"));
}
BOOST_FIXTURE_TEST_CASE(_3_node_1_tmpl, trees_fixture)
{
	map_to mapper;
	t1_root = mk_node({.version=1, .node_var=variable{"var1", "val1"}});
	mk_tree(t1(), {
	            {std::nullopt, {.node_var = variable{"var2", "val2"}}},
	            {std::nullopt, {.node_var = variable{"var3", "val3"}}}
	        });
	auto r = mapper("_${var1}_", t1());
	BOOST_TEST(r.size() == 1) ;
	BOOST_CHECK(r.contains("_val1_")) ;
}
BOOST_FIXTURE_TEST_CASE(few_vars, trees_fixture)
{
	mk_tree(t1(), {
	            {std::nullopt, {.node_var=variable{"var1", "val1"}}}
	          , {std::nullopt, {.node_var=variable{"var2", "val2"}}}
	          , { -1, {.node_var=variable{"var2", "val21"}}}
	          , {std::nullopt, {.node_var=variable{"var3", "val3"}}}
	          , { -1, {.node_var=variable{"var3", "val31"}}}
	        });
	auto r = map_to()("_${var1}_${var2}_${var3}_${var1}_", t1());
	BOOST_TEST(r.size()==4);
	BOOST_CHECK(r.contains("_val1_val2_val3_val1_"));
	BOOST_CHECK(r.contains("_val1_val2_val31_val1_"));
	BOOST_CHECK(r.contains("_val1_val21_val3_val1_"));
	BOOST_CHECK(r.contains("_val1_val21_val31_val1_"));
}
BOOST_FIXTURE_TEST_CASE(no_var, trees_fixture)
{
	t1_root = mk_node({.version=1, .node_var=variable{"var1", "val1"}});
	auto r = map_to()("_${var1}_${var2}_", t1());
	BOOST_TEST(r.size()==1);
	BOOST_CHECK(r.contains("_val1_${var2}_"));
}
BOOST_FIXTURE_TEST_CASE(no_any_var, trees_fixture)
{
	t1_root = mk_node({.version=1});
	auto r = map_to()("_${var1}_${var2}_", t1());
	BOOST_TEST(r.size()==1);
	BOOST_CHECK(r.contains("_${var1}_${var2}_"));
}
BOOST_FIXTURE_TEST_CASE(no_var_no_ref, trees_fixture)
{
	t1_root = mk_node({.version=1});
	auto r = map_to()("_xxx_", t1());
	BOOST_TEST(r.size()==1);
	BOOST_CHECK(r.contains("_xxx_"));
}
BOOST_FIXTURE_TEST_CASE(double_use, trees_fixture)
{
	map_to mapper;
	t1_root = mk_node({.version=1, .node_var=variable{"var", "val"}});
	auto r = mapper("_xxx_", t1());
	BOOST_TEST(r.size()==1);
	BOOST_CHECK(r.contains("_xxx_"));

	r = mapper("_${var}_", t1());
	BOOST_TEST(r.size()==1);
	BOOST_CHECK(r.contains("_val_"));
}
BOOST_FIXTURE_TEST_CASE(for_input, trees_fixture)
{
	map_to mapper;
	gen_utils::input all_data;
	mk_tree(t1(), {
	            {std::nullopt, {.version=1, .node_var=variable{"n", "n1"}}}
	          , {std::nullopt, {.version=1, .node_var=variable{"n", "n2"}}}
	          , {std::nullopt, {.version=2, .node_var=variable{"m", "m1"}}}
	          , {std::nullopt, {.version=2, .node_var=variable{"m", "m2"}}}
	        });
	all_data.add(t1());
	auto r = mapper("${n}.file", all_data);
	BOOST_TEST(r.size() == 2);
	BOOST_CHECK(r.contains("n1.file"));
	BOOST_CHECK(r.contains("n2.file"));

	mk_tree(t2(), {{std::nullopt, {}}, {0, {}}, {std::nullopt, {}}});
	all_data.add(t2());

	r = mapper("${n}.file", all_data);
	BOOST_TEST(r.size() == 2);
	BOOST_CHECK(r.contains("n1.file"));
	BOOST_CHECK(r.contains("n2.file"));
	BOOST_TEST(r["n1.file"].all().size() == 2);

	r = mapper("${n}_${m}", all_data);
	BOOST_TEST(r.size() == 4);
	BOOST_CHECK(r.contains("n1_m1"));
	BOOST_CHECK(r.contains("n1_m2"));
	BOOST_CHECK(r.contains("n2_m1"));
	BOOST_CHECK(r.contains("n2_m2"));
}
BOOST_FIXTURE_TEST_CASE(few_levels, trees_fixture)
{
	mk_tree(t1(), {
	            {std::nullopt, {.version=1, .node_var=variable{"mod", "mod1"}}}
	          , {std::nullopt, {.version=1, .node_var=variable{"mod", "mod2"}}}
	          , { 0, {.version=1, .node_var=variable{"a", "1"}}}
	          , {-1, {.version=1, .node_var=variable{"i", "0"}}}
	          , { 1, {.version=1, .node_var=variable{"a", "2"}}}
	          , {-1, {.version=1, .node_var=variable{"i", "1"}}}
	        });

	auto r = map_to()("_${mod}_${a}_${i}_", t1());
	BOOST_TEST(r.size() == 2);
	BOOST_CHECK(r.contains("_mod1_1_0_"));
	BOOST_CHECK(r.contains("_mod2_2_1_"));

	r = map_to()("_${i}_${mod}_${a}_${i}_", t1());
	BOOST_TEST(r.size() == 2);
	BOOST_CHECK(r.contains("_0_mod1_1_0_"));
	BOOST_CHECK(r.contains("_1_mod2_2_1_"));
}
BOOST_AUTO_TEST_SUITE_END() // tree_map_to

BOOST_AUTO_TEST_SUITE(tree_map_from)
BOOST_FIXTURE_TEST_CASE(simple, trees_fixture)
{
	using gen_utils_mocks::check_vec_eq;

	map_to mapper;
	t1().add(*t1_root, make_node(1, "var1", "v11"));
	t1().add(*t1_root, make_node(1, "var1", "v12"));
	t1().add(*t1_root, make_node(1, "var2", "v21"));
	t1().add(*t1_root, make_node(1, "var2", "v22"));
	t1().add(*t1_root, make_node(1, "var3", "v31"));
	t1().add(*t1_root, make_node(1, "var3", "v32"));

	auto map_result = mapper("_${var1}_${var2}_${var3}_", t1());
	std::vector<std::pmr::string> map_names;
	for(auto& [n,_]:map_result) map_names.emplace_back(n);

	map_from demapper;
	BOOST_TEST(demapper(map_result, "_${var1}_", t1()).size() == 2);
	BOOST_TEST(demapper(map_result, "_${var1}_", t1()).at("_v11_").size() == 4);
	BOOST_TEST(demapper(map_result, "_${var1}_", t1()).at("_v12_").size() == 4);
	check_vec_eq(
		demapper(map_result, "_${var1}_", t1()).at("_v11_"),
		{"_v11_v21_v31_"_s, "_v11_v21_v32_"_s, "_v11_v22_v31_"_s, "_v11_v22_v32_"_s});
	check_vec_eq(
		demapper(map_result, "_${var1}_", t1()).at("_v12_"),
		{"_v12_v21_v31_"_s, "_v12_v21_v32_"_s, "_v12_v22_v31_"_s, "_v12_v22_v32_"_s});

	BOOST_TEST(demapper(map_result, "_${var2}_", t1()).size() == 2);
	BOOST_TEST(demapper(map_result, "_${var2}_", t1()).at("_v21_").size() == 4);
	BOOST_TEST(demapper(map_result, "_${var2}_", t1()).at("_v22_").size() == 4);
	check_vec_eq(
		demapper(map_result, "_${var2}_", t1()).at("_v21_"),
		{"_v11_v21_v31_"_s, "_v11_v21_v32_"_s, "_v12_v21_v31_"_s, "_v12_v21_v32_"_s});
	check_vec_eq(
		demapper(map_result, "_${var2}_", t1()).at("_v22_"),
		{"_v11_v22_v31_"_s, "_v11_v22_v32_"_s, "_v12_v22_v31_"_s, "_v12_v22_v32_"_s});

	BOOST_TEST(demapper(map_result, "__", t1()).size() == 1);
	check_vec_eq(demapper(map_result, "__", t1()).at("__"), map_names);
	BOOST_TEST(demapper(map_result, "__", t1()).at("__").size() == 8);
}
BOOST_FIXTURE_TEST_CASE(for_input, trees_fixture)
{
	map_to mapper;
	t1().add(*t1_root, make_node(1, "var1", "v11"));
	t1().add(*t1_root, make_node(1, "var1", "v12"));
	t1().add(*t1_root, make_node(1, "var2", "v21"));
	t1().add(*t1_root, make_node(1, "var2", "v22"));
	t2().add(*t2_root, make_node(2, "var3", "v31"));
	t2().add(*t2_root, make_node(2, "var3", "v32"));
	t3().add(*t3_root, make_node(3, "var4", "v41"));
	gen_utils::input all_data;
	all_data.add(t1()).add(t2());

	auto map_result = mapper("_${var1}_${var2}_${var3}_", all_data);
	std::vector<std::pmr::string> map_names;
	for(auto& [n,_]:map_result) map_names.emplace_back(n);

	map_from demapper;
	BOOST_TEST(demapper(map_result, "_${var1}_", all_data).size() == 2);
	check_vec_eq(
	    demapper(map_result, "_${var1}_", all_data).at("_v11_"),
	    {"_v11_v21_v31_"_s, "_v11_v21_v32_"_s, "_v11_v22_v31_"_s, "_v11_v22_v32_"_s});
	check_vec_eq(
	    demapper(map_result, "_${var1}_", all_data).at("_v12_"),
	    {"_v12_v21_v31_"_s, "_v12_v21_v32_"_s, "_v12_v22_v31_"_s, "_v12_v22_v32_"_s});
	BOOST_TEST(demapper(map_result, "__", all_data).size() == 1);
	all_data.add(t3());
	BOOST_TEST(demapper(map_result, "_${var1}_", all_data).size() == 2);
	check_vec_eq(
	    demapper(map_result, "_${var1}_", all_data).at("_v12_"),
	    {"_v12_v21_v31_"_s, "_v12_v21_v32_"_s, "_v12_v22_v31_"_s, "_v12_v22_v32_"_s});
}
BOOST_FIXTURE_TEST_CASE(for_input_single_var, trees_fixture)
{
	t1().add(t1().root(), make_node(1, "v1", "m1"));
	t1().add(t1().root(), make_node(1, "v1", "m2"));
	t2().add(t2().root(), make_node(2, "v2", "n1"));
	t2().add(t2().root(), make_node(2));
	gen_utils::input all_data;
	all_data.add(t1()).add(t2());

	auto map_result = map_to()("f_${v1}", all_data);

	auto dr = map_from()(map_result, "f_${v2}", all_data);
	BOOST_TEST(dr.size()==1);
	check_vec_eq(dr.at("f_n1"), {"f_m1"_s, "f_m2"_s});

	dr = map_from()(map_result, "f", all_data);
	BOOST_TEST(dr.size()==1);
	check_vec_eq(dr.at("f"), {"f_m1"_s, "f_m2"_s});
}
BOOST_AUTO_TEST_SUITE_END() // tree_map_from

BOOST_AUTO_TEST_SUITE_END() // input
