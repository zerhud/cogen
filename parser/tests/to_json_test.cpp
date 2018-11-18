#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE modegen_parser

#include <iostream>
#include <boost/test/unit_test.hpp>

#include "to_json.h"
#include "grammar.hpp"

using namespace std::literals;

BOOST_AUTO_TEST_CASE(empty)
{
	auto mods = modegen::parse("module mod v1: #help mes\nmodule mod2 v1:");
	BOOST_REQUIRE_EQUAL(mods.size(), 2);
	std::string result = modegen::converters::to_json(mods);
	BOOST_CHECK_EQUAL(result, R"-([{"name":"mod","type":"module","v":1},{"docs":"help mes","name":"mod2","type":"module","v":1}])-");
}

BOOST_AUTO_TEST_CASE(fnc)
{
	auto mods = modegen::parse("module mod v1: #help mes\n@v2 type some();");
	BOOST_REQUIRE_EQUAL(mods.size(), 1);
	cppjson::value result = modegen::converters::to_json(mods);
	BOOST_CHECK_EQUAL(result[0]["name"], "mod");
	BOOST_CHECK_EQUAL(result[0]["content"][0]["name"], "some");
}

BOOST_AUTO_TEST_CASE(enums)
{
	auto mods = modegen::parse("module mod v1: enum some{one two};");
	BOOST_REQUIRE_EQUAL(mods.size(), 1);
	cppjson::value result = modegen::converters::to_json(mods);
	cppjson::value& enum_rec = result[0]["content"][0];
	BOOST_CHECK_EQUAL(enum_rec["name"], "some");
	BOOST_CHECK_EQUAL(enum_rec["type"], "enumeration");
	BOOST_CHECK_EQUAL(enum_rec["gen_io"], false);
	BOOST_CHECK_EQUAL(enum_rec["use_bitmask"], false);
	BOOST_REQUIRE_EQUAL(enum_rec["members"].array().size(), 2);
	BOOST_CHECK_EQUAL(enum_rec["members"][0]["name"], "one");
	BOOST_CHECK_EQUAL(enum_rec["members"][0]["output"], "");
	BOOST_CHECK_EQUAL(enum_rec["members"][1]["name"], "two");
	BOOST_CHECK_EQUAL(enum_rec["members"][1]["output"], "");
	std::cout << result << std::endl;
}
