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

std::pmr::string operator "" _s (const char* d, std::size_t l)
{
	return std::pmr::string(d, l);
}

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
using gen_utils_mocks::make_node;
using fixture = gen_utils_mocks::base_tree_fixture;
using gen_utils_mocks::trees_fixture;

BOOST_AUTO_TEST_SUITE(tree_map_to)
BOOST_FIXTURE_TEST_CASE(one_var, fixture)
{
	map_to mapper;
	main_node = make_node(1, "var", "val");
	auto r = mapper("_${var}_", tree());
	BOOST_TEST(r.size()==1);
	BOOST_CHECK(r.contains("_val_"));
}
BOOST_FIXTURE_TEST_CASE(tmpl_diff_from_var_name, fixture)
{
	map_to mapper;
	main_node = make_node(1, "var", "val");
	auto r = mapper("_${var1}_", tree());
	BOOST_TEST(r.size()==1);
	BOOST_CHECK(r.contains("_${var1}_"));
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
	BOOST_CHECK(r.contains("_val1_")) ;
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
	BOOST_CHECK(r.contains("_val1_val2_val3_val1_"));
	BOOST_CHECK(r.contains("_val1_val2_val31_val1_"));
	BOOST_CHECK(r.contains("_val1_val21_val3_val1_"));
	BOOST_CHECK(r.contains("_val1_val21_val31_val1_"));
}
BOOST_FIXTURE_TEST_CASE(no_var, fixture)
{
	map_to mapper;
	main_node = make_node(1, "var1", "val1");
	auto r = mapper("_${var1}_${var2}_", tree());
	BOOST_TEST(r.size()==1);
	BOOST_CHECK(r.contains("_val1_${var2}_"));
}
BOOST_FIXTURE_TEST_CASE(no_any_var, fixture)
{
	map_to mapper;
	main_node = make_node(1);
	auto r = mapper("_${var1}_${var2}_", tree());
	BOOST_TEST(r.size()==1);
	BOOST_CHECK(r.contains("_${var1}_${var2}_"));
}
BOOST_FIXTURE_TEST_CASE(no_var_no_ref, fixture)
{
	map_to mapper;
	main_node = make_node(1);
	auto r = mapper("_xxx_", tree());
	BOOST_TEST(r.size()==1);
	BOOST_CHECK(r.contains("_xxx_"));
}
BOOST_FIXTURE_TEST_CASE(double_use, fixture)
{
	map_to mapper;
	main_node = make_node(1, "var", "val");
	auto r = mapper("_xxx_", tree());
	BOOST_TEST(r.size()==1);
	BOOST_CHECK(r.contains("_xxx_"));

	r = mapper("_${var}_", tree());
	BOOST_TEST(r.size()==1);
	BOOST_CHECK(r.contains("_val_"));
}
BOOST_FIXTURE_TEST_CASE(for_input, trees_fixture)
{
	map_to mapper;
	gen_utils::input all_data;
	t1().add(t1().root(), make_node(1, "n", "n1"));
	t1().add(t1().root(), make_node(1, "n", "n2"));
	t2().add(t2().root(), make_node(2, "m", "m1"));
	t2().add(t2().root(), make_node(2, "m", "m2"));
	all_data.add(t1());
	auto r = mapper("${n}.file", all_data);
	BOOST_TEST(r.size() == 2);
	BOOST_CHECK(r.contains("n1.file"));
	BOOST_CHECK(r.contains("n2.file"));

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
}
BOOST_AUTO_TEST_SUITE_END() // tree_map_from

BOOST_AUTO_TEST_SUITE_END() // input
