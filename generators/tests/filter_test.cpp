#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE module_filter

#include <boost/test/unit_test.hpp>

#include "parser/grammar.hpp"
#include "generator.hpp"

using namespace std::literals;

BOOST_AUTO_TEST_SUITE(module_name)
BOOST_AUTO_TEST_CASE(no_filter)
{
	auto pf = modegen::parse("module mod v1.0: enum e{}"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),1);
	modegen::mod_selection query;
	modegen::filter_by_selection(query, pf.mods);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),1);
	BOOST_CHECK_EQUAL(pf.mods[0].content.size(),1);
}
BOOST_AUTO_TEST_CASE(exactly)
{
	auto pf = modegen::parse("module mod v1.0: enum e{} module mod1 v1.0: enum e{}"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),2);
	modegen::mod_selection query;
	query.mod_name = "mod";
	modegen::filter_by_selection(query, pf.mods);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),1);
	BOOST_CHECK_EQUAL(pf.mods[0].content.size(),1);
	BOOST_CHECK_EQUAL(pf.mods[0].name,"mod"sv);
}
BOOST_AUTO_TEST_CASE(pattern)
{
	auto pf = modegen::parse("module mod v1.0: enum e{} module mad1 v1.0: enum e{} module mod1 v1.0: enum e{}"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),3);
	modegen::mod_selection query;
	query.mod_name = "mo.*";
	modegen::filter_by_selection(query, pf.mods);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),2);
	BOOST_CHECK_EQUAL(pf.mods[0].content.size(),1);
	BOOST_CHECK_EQUAL(pf.mods[0].name,"mod"sv);
	BOOST_CHECK_EQUAL(pf.mods[1].name,"mod1"sv);
}
BOOST_AUTO_TEST_SUITE_END() // module_name
