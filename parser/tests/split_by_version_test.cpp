#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE modegen_parser

#include <boost/test/unit_test.hpp>

#include "split_by_version.hpp"
#include "grammar.hpp"

using namespace std::literals;

BOOST_AUTO_TEST_CASE(functions)
{
	auto mods = modegen::parse("module mod v1.0: type name(); @v2.0 type name();"sv);
	BOOST_REQUIRE_EQUAL(mods.size(), 1);

	modegen::split_by_version sp;
	sp(mods);
	BOOST_REQUIRE_EQUAL(mods.size(), 2);
}
