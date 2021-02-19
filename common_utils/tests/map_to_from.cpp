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
using gen_utils_mocks::trees_fixture;

std::pmr::string operator "" _s (const char* d, std::size_t l)
{
	return std::pmr::string(d, l);
}

std::ostream& operator << (std::ostream& out, const gen_utils::map_to::result_t& mr)
{
	for(auto& v:mr) out << v.first << ' ';
	return out;
}

std::ostream& operator << (std::ostream& out, const std::pmr::vector<std::pmr::string>& vec)
{
	for(auto& v:vec) out << v << ' ' ;
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
BOOST_AUTO_TEST_SUITE_END() // tree_map_to

BOOST_AUTO_TEST_SUITE(tree_map_from)
BOOST_FIXTURE_TEST_CASE(simple, trees_fixture)
{
	map_to mapper;
	t1().add(*t1_root, make_node(1, "var1", "v11"));
	t1().add(*t1_root, make_node(1, "var1", "v12"));
	t1().add(*t1_root, make_node(1, "var2", "v21"));
	t1().add(*t1_root, make_node(1, "var2", "v22"));
	auto r = mapper("_${var1}_${var2}_", t1());
	std::cout << __LINE__ << ' ' << r << std::endl;

	map_from demapper;
	std::cout << "var1: ";
	for(auto& v:demapper(r, "_${var1}_", t1())) std::cout << v << ' ';
	std::cout << std::endl;
	std::cout << "var2: ";
	for(auto& v:demapper(r, "_${var2}_", t1())) std::cout << v << ' ';
	std::cout << std::endl;
	BOOST_TEST(demapper(r, "_${var1}_", t1()).size() == 2);
	BOOST_TEST(demapper(r, "_${var2}_", t1()).size() == 2);
	BOOST_TEST(demapper(r, "__", t1()).size() == 1);
}
BOOST_AUTO_TEST_SUITE_END() // tree_map_from

BOOST_AUTO_TEST_SUITE_END() // input