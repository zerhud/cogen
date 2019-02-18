/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE module_filter

#include <boost/test/unit_test.hpp>

#include "parser/interface/grammar.hpp"
#include "generation/interface/filter.hpp"

using namespace std::literals;
using modegen::generation::interface::filter;
namespace mi = modegen::parser::interface;

BOOST_AUTO_TEST_SUITE(module_name)
BOOST_AUTO_TEST_CASE(no_filter)
{
	auto pf = mi::parse("module mod v1.0: enum e{}"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),1);
	filter::request query;
	filter{query}(pf.mods);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),1);
	BOOST_CHECK_EQUAL(pf.mods[0].content.size(),1);
}
BOOST_AUTO_TEST_CASE(exactly)
{
	auto pf = mi::parse("module mod v1.0: enum e{} module mod1 v1.0: enum e{}"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),2);
	filter::request query;
	query.mod_name = "mod";
	filter{query}(pf.mods);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),1);
	BOOST_CHECK_EQUAL(pf.mods[0].content.size(),1);
	BOOST_CHECK_EQUAL(pf.mods[0].name,"mod"sv);
}
BOOST_AUTO_TEST_CASE(pattern)
{
	auto pf = mi::parse("module mod v1.0: enum e{} module mad1 v1.0: enum e{} module mod1 v1.0: enum e{}"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),3);
	filter::request query;
	query.mod_name = "mo.*";
	filter{query}(pf.mods);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),2);
	BOOST_CHECK_EQUAL(pf.mods[0].content.size(),1);
	BOOST_CHECK_EQUAL(pf.mods[0].name,"mod"sv);
	BOOST_CHECK_EQUAL(pf.mods[1].name,"mod1"sv);
}
BOOST_AUTO_TEST_SUITE_END() // module_name

BOOST_AUTO_TEST_SUITE(content_name)
BOOST_AUTO_TEST_CASE(pattern)
{
	auto pf = mi::parse("module mod v1.0: enum e1{} enum e2{} enum a3{}"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].content.size(),3);
	filter::request query;
	query.cnt_name = "e[0-9]+";
	filter{query}(pf.mods);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),1);
	std::vector<mi::module_content>& cnt = pf.mods[0].content;
	BOOST_CHECK_EQUAL(cnt.size(),2);

	namespace pl = std::placeholders;
	auto ncheck = [](const auto& n, const auto& c){ BOOST_CHECK_EQUAL(c.name,n); };
	std::visit(std::bind(ncheck, "e1", pl::_1), cnt[0]);
	std::visit(std::bind(ncheck, "e2", pl::_1), cnt[1]);
}
BOOST_AUTO_TEST_SUITE_END() // content_name
