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

BOOST_AUTO_TEST_SUITE(simple)
BOOST_AUTO_TEST_CASE(functions)
{
	auto presult = mi::parse("module mod v1.0: type low(); module mod v1.1: type low(); type high();"sv);

	auto pr = mi::parse("module mod v1.0: type low(); type high();"sv);
	mg::split_version{}(pr.mods);
	BOOST_CHECK(presult.mods == pr.mods);

	pr = mi::parse("module mod v1.0: type high(); type low();"sv);
	mg::split_version{}(pr.mods);
	BOOST_CHECK(presult.mods == pr.mods);
}
BOOST_AUTO_TEST_CASE(record)
{
	auto pr = mi::parse("module mod v1.0: record r1 {type f1; @v1.1 type f2;}"sv);

	mg::split_version sp;
	sp(pr.mods);

	auto presult = mi::parse("module mod v1.0: record r1 {type f1;} module mod v1.1: record r1{ type f1; type f2;}"sv);
	BOOST_CHECK(presult.mods == pr.mods);
}
BOOST_AUTO_TEST_SUITE_END() // simple

