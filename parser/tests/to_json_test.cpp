#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE modegen_parser

#include <iostream>
#include <boost/test/unit_test.hpp>

#include "to_json.h"
#include "grammar.hpp"

using namespace std::literals;

void check_type(const cppjson::value& v, const std::string& name, const std::string& mod)
{
	BOOST_CHECK_EQUAL(v["name"], name);
	BOOST_CHECK_EQUAL(v["modificator"], mod);
	BOOST_CHECK_EQUAL(v["type"], "type");
}

BOOST_AUTO_TEST_CASE(empty)
{
	auto mods = modegen::parse("module mod v1: #help mes\nmodule mod2 v1:");
	BOOST_REQUIRE_EQUAL(mods.size(), 2);
	std::string result = modegen::converters::to_json(mods);
	BOOST_CHECK_EQUAL(result, R"-([{"name":"mod","type":"module","v":1},{"docs":"help mes","name":"mod2","type":"module","v":1}])-");
}

BOOST_AUTO_TEST_CASE(fnc)
{
	auto mods = modegen::parse("module mod v1: #help mes\n@v2 type some(type one, type two);");
	BOOST_REQUIRE_EQUAL(mods.size(), 1);
	cppjson::value result = modegen::converters::to_json(mods);
	BOOST_CHECK_EQUAL(result[0]["name"], "mod");
	BOOST_CHECK_EQUAL(result[0]["v"], 1);

	cppjson::value& fnc = result[0]["content"][0];
	BOOST_CHECK_EQUAL(fnc["name"], "some");
	BOOST_CHECK_EQUAL(fnc["type"], "function");
	BOOST_CHECK_EQUAL(fnc["v"], 2);
	BOOST_CHECK(fnc["static"].is_null());
	BOOST_CHECK(fnc["mutable"].is_null());
	BOOST_REQUIRE_EQUAL(fnc["params"].array().size(), 2);
	BOOST_CHECK_EQUAL(fnc["params"][0]["name"], "one");
	BOOST_CHECK_EQUAL(fnc["params"][1]["name"], "two");
	check_type(fnc["params"][0]["par_type"], "type", "");
	check_type(fnc["params"][1]["par_type"], "type", "");
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
}

BOOST_AUTO_TEST_CASE(record)
{
	auto mods = modegen::parse("module mod v1: record rec {}");
	BOOST_REQUIRE_EQUAL(mods.size(), 1);
	cppjson::value result = modegen::converters::to_json(mods);
	BOOST_CHECK_EQUAL(result[0]["name"], "mod");
	BOOST_CHECK_EQUAL(result[0]["v"], 1);

	cppjson::value& rec = result[0]["content"][0];
	BOOST_CHECK(rec["v"].is_null());
	BOOST_CHECK_EQUAL(rec["name"], "rec");
	BOOST_CHECK_EQUAL(rec["type"], "record");

	std::cout << result << std::endl;
}
