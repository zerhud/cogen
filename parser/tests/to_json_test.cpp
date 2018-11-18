#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE modegen_parser

#include <boost/test/unit_test.hpp>

#include "to_json.h"
#include "grammar.hpp"

using namespace std::literals;

BOOST_AUTO_TEST_CASE(empty)
{
	auto mods = modegen::parse("module mod v1: #help mes\nmodule mod2 v1:");
	BOOST_REQUIRE_EQUAL(mods.size(), 2);
	std::string result = modegen::converters::to_json(mods);
	BOOST_CHECK_EQUAL(result, R"-([{"name":"mod","v":1},{"docs":"help mes","name":"mod2","v":1}])-");
}
