#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE module_filter

#include <boost/test/unit_test.hpp>

#include "parser/grammar.hpp"
#include "generator.hpp"
#include "converters/filter.h"

using namespace std::literals;

BOOST_AUTO_TEST_SUITE(module_name)
BOOST_AUTO_TEST_CASE(no_filter)
{
	auto pf = modegen::parse("module mod v1.0: enum e{}"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),1);
	modegen::generation_request query;
	modegen::cvt::filter{query}(pf.mods);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),1);
	BOOST_CHECK_EQUAL(pf.mods[0].content.size(),1);
}
BOOST_AUTO_TEST_CASE(exactly)
{
	auto pf = modegen::parse("module mod v1.0: enum e{} module mod1 v1.0: enum e{}"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),2);
	modegen::generation_request query;
	query.mod_name = "mod";
	modegen::cvt::filter{query}(pf.mods);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),1);
	BOOST_CHECK_EQUAL(pf.mods[0].content.size(),1);
	BOOST_CHECK_EQUAL(pf.mods[0].name,"mod"sv);
}
BOOST_AUTO_TEST_CASE(pattern)
{
	auto pf = modegen::parse("module mod v1.0: enum e{} module mad1 v1.0: enum e{} module mod1 v1.0: enum e{}"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),3);
	modegen::generation_request query;
	query.mod_name = "mo.*";
	modegen::cvt::filter{query}(pf.mods);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),2);
	BOOST_CHECK_EQUAL(pf.mods[0].content.size(),1);
	BOOST_CHECK_EQUAL(pf.mods[0].name,"mod"sv);
	BOOST_CHECK_EQUAL(pf.mods[1].name,"mod1"sv);
}
BOOST_AUTO_TEST_SUITE_END() // module_name

BOOST_AUTO_TEST_SUITE(content_name)
BOOST_AUTO_TEST_CASE(pattern)
{
	auto pf = modegen::parse("module mod v1.0: enum e1{} enum e2{} enum a3{}"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].content.size(),3);
	modegen::generation_request query;
	query.cnt_name = "e[0-9]+";
	modegen::cvt::filter{query}(pf.mods);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),1);
	std::vector<modegen::module_content>& cnt = pf.mods[0].content;
	BOOST_CHECK_EQUAL(cnt.size(),2);

	namespace pl = std::placeholders;
	auto ncheck = [](const auto& n, const auto& c){ BOOST_CHECK_EQUAL(c.name,n); };
	std::visit(std::bind(ncheck, "e1", pl::_1), cnt[0]);
	std::visit(std::bind(ncheck, "e2", pl::_1), cnt[1]);
}
BOOST_AUTO_TEST_SUITE_END() // content_name
