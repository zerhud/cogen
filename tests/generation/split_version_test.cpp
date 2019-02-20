/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE modegen_parser

#include <boost/test/unit_test.hpp>

#include "generation/interface/split_version.hpp"
#include "parser/interface/grammar.hpp"
#include "parser/interface/helpers.hpp"

using namespace std::literals;
namespace mi = modegen::parser::interface;
namespace mg = modegen::generation::interface;
using modegen::parser::interface::meta_parameters::version;

BOOST_AUTO_TEST_SUITE(simple)
BOOST_AUTO_TEST_CASE(functions)
{
	auto presult = mi::parse("module mod v1.0: type low(); module mod v1.1: type low(); type high();"sv);
	auto pr = mi::parse("module mod v1.0: type low(); @v1.1 type high();"sv);
	mg::split_version{}(pr.mods);
	BOOST_CHECK(presult.mods == pr.mods);
	BOOST_CHECK_EQUAL(pr.mods.size(), 2);

	presult = mi::parse("module mod v1.0: type low(); module mod v1.1: type high(); type low();"sv);
	pr = mi::parse("module mod v1.0: @v1.1 type high(); type low();"sv);
	mg::split_version{}(pr.mods);
	BOOST_CHECK(presult.mods == pr.mods);
	BOOST_CHECK_EQUAL(pr.mods.size(), 2);
}
BOOST_AUTO_TEST_CASE(record)
{
	auto pr = mi::parse("module mod v1.0: record r1 {type f1; @v1.1 type f2;}"sv);

	mg::split_version sp;
	sp(pr.mods);

	auto presult = mi::parse("module mod v1.0: record r1 {type f1;} module mod v1.1: record r1{ type f1; type f2;}"sv);
	BOOST_CHECK(presult.mods == pr.mods);
	BOOST_CHECK_EQUAL(pr.mods.size(), 2);
	BOOST_CHECK_EQUAL(pr.mods[0].content.size(), 1);
	BOOST_CHECK_EQUAL(pr.mods[1].content.size(), 1);
	mi::record& rfirst = std::get<mi::record>(pr.mods[0].content[0]);
	mi::record& rlast = std::get<mi::record>(pr.mods[1].content[0]);
	BOOST_CHECK( !mi::get<version>(rlast) );
	BOOST_CHECK_EQUAL(rfirst.members.size(), 1);
	BOOST_CHECK_EQUAL(rlast.members.size(), 2);
	BOOST_CHECK_EQUAL(rlast.members[1].meta_params.set.size(), 0);
}
BOOST_AUTO_TEST_CASE(interface)
{
	auto pr = mi::parse("module mod v1.0: interface r1 {type func1() const; @v1.3 type func2() const;}"sv);
	auto presult = mi::parse("module mod v1.0: interface r1 {type func1()const;} module mod v1.3: interface r1{ type func1()const; type func2()const;}"sv);
	mg::split_version{}(pr.mods);
	BOOST_CHECK(presult.mods == pr.mods);
}
BOOST_AUTO_TEST_SUITE_END() // simple

BOOST_AUTO_TEST_CASE(with_empty)
{
	auto pr = mi::parse("module mod v1.0: @v1.1 record r1 {@v1.2 type f2; @v1.3 type f3;}"sv);
	auto presult = mi::parse("module mod v1.0: module mod v1.1: record r1{} module mod v1.2:record r1{type f2;}module mod v1.3:record r1{type f2;type f3;}");
	mg::split_version{}(pr.mods);
	BOOST_CHECK(presult.mods == pr.mods);
	BOOST_CHECK_EQUAL(presult.mods.size(), 4);
}
