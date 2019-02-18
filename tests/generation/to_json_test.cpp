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

#include "generation/interface/to_json.hpp"
#include "parser/interface/grammar.hpp"

using namespace std::literals;
namespace mi = modegen::parser::interface;
namespace mg = modegen::generation::interface;

struct test_json_asp : mg::to_json_aspect {
	void as_object(cppjson::value& jval, const modegen::parser::interface::module& obj) override
	{
		jval["mn"] = obj.name;
	}
};

void check_type(const cppjson::value& v, const std::string& name, const std::string& mod)
{
	BOOST_CHECK_EQUAL(v["type"], "type");

	if(mod.empty()) {
		BOOST_CHECK_EQUAL(v["sub"].type(), cppjson::is_null);
		BOOST_CHECK_EQUAL(v["name"], name);
	}
	else {
		BOOST_CHECK_EQUAL(v["sub"].type(), cppjson::is_array);
		BOOST_CHECK_EQUAL(v["name"], mod);
		BOOST_CHECK_EQUAL(v["sub"][0]["name"], name);
	}
}

BOOST_AUTO_TEST_CASE(empty)
{
	auto pf = mi::parse("module mod v1.0: #help mes\nmodule mod2 v1.0:");
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 2);
	std::string result = mg::to_json()(pf.mods);
	BOOST_CHECK_EQUAL(result, R"-({"mods":[{"name":"mod","type":"module","v":{"major":1,"minor":0}},{"docs":"help mes","name":"mod2","type":"module","v":{"major":1,"minor":0}}]})-");
}

BOOST_AUTO_TEST_CASE(fnc)
{
	auto pf = mi::parse("module mod v1.0: #help mes\n@v2.10 type some(type one, type two); type some2();");
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].content.size(), 2);

	cppjson::value result_ = mg::to_json()(pf.mods);
	cppjson::value result = result_["mods"];
	BOOST_CHECK_EQUAL(result[0]["name"], "mod");
	BOOST_CHECK_EQUAL(result[0]["v"]["major"], 1);
	BOOST_CHECK_EQUAL(result[0]["v"]["minor"], 0);

	cppjson::value& fnc = result[0]["content"][0];
	BOOST_CHECK_EQUAL(fnc["name"], "some");
	BOOST_CHECK_EQUAL(fnc["type"], "function");
	BOOST_CHECK_EQUAL(fnc["v"]["major"], 2);
	BOOST_CHECK_EQUAL(fnc["v"]["minor"], 10);
	BOOST_CHECK(fnc["static"].is_null());
	BOOST_CHECK(fnc["mutable"].is_null());
	BOOST_REQUIRE_EQUAL(fnc["params"].array().size(), 2);
	BOOST_CHECK_EQUAL(fnc["params"][0]["name"], "one");
	BOOST_CHECK_EQUAL(fnc["params"][1]["name"], "two");
	check_type(fnc["params"][0]["par_type"], "type", "");
	check_type(fnc["params"][1]["par_type"], "type", "");

	cppjson::value& fnc2 = result[0]["content"][1];
	BOOST_REQUIRE_EQUAL(fnc2["params"].type(), cppjson::is_array);
	BOOST_REQUIRE_EQUAL(fnc2["params"].array().size(), 0);
}

BOOST_AUTO_TEST_CASE(enums)
{
	auto pf = mi::parse("module mod v1.0: enum some{one two};");
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 1);
	cppjson::value result_ = mg::to_json()(pf.mods);
	cppjson::value result = result_["mods"];

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
	auto pf = mi::parse("module mod v1.2: record rec { type m1; list<other_type> m2;}");
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 1);
	cppjson::value result_ = mg::to_json()(pf.mods);
	cppjson::value result = result_["mods"];

	BOOST_CHECK_EQUAL(result[0]["name"], "mod");
	BOOST_CHECK_EQUAL(result[0]["v"]["major"], 1);
	BOOST_CHECK_EQUAL(result[0]["v"]["minor"], 2);

	cppjson::value& rec = result[0]["content"][0];
	BOOST_CHECK(rec["v"].is_null());
	BOOST_CHECK_EQUAL(rec["name"], "rec");
	BOOST_CHECK_EQUAL(rec["type"], "record");

	mi::type& m2type = std::get<mi::record>(pf.mods[0].content[0]).members[1].param_type;
	BOOST_CHECK_EQUAL(m2type.name, "list");

	BOOST_REQUIRE_EQUAL(rec["members"].type(), cppjson::is_array);
	BOOST_REQUIRE_EQUAL(rec["members"].array().size(), 2);
	BOOST_CHECK_EQUAL(rec["members"][0]["name"], "m1");
	BOOST_CHECK_EQUAL(rec["members"][1]["name"], "m2");
	check_type(rec["members"][0]["par_type"], "type", "");
	check_type(rec["members"][1]["par_type"], "other_type", "list");
}

BOOST_AUTO_TEST_CASE(interface)
{
	auto pf = mi::parse("module mod v1.0: interface i { type name() mutable ; constructor(); constructor(type name); }");
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].content.size(), 1);

	cppjson::value result_ = mg::to_json()(pf.mods);
	cppjson::value result = result_["mods"];

	BOOST_CHECK_EQUAL(result[0]["name"], "mod");
	BOOST_CHECK_EQUAL(result[0]["v"]["major"], 1);
	BOOST_CHECK_EQUAL(result[0]["v"]["minor"], 0);

	cppjson::value& i = result[0]["content"][0];
	BOOST_CHECK_EQUAL(i["name"], "i");
	BOOST_CHECK_EQUAL(i["type"], "interface");
	BOOST_CHECK_EQUAL(i["invert"], false);

	BOOST_REQUIRE_EQUAL(i["members"].type(), cppjson::is_array);
	BOOST_REQUIRE_EQUAL(i["members"].array().size(), 1);
	BOOST_CHECK_EQUAL(i["members"][0]["name"], "name");

	BOOST_REQUIRE_EQUAL(i["constructors"].type(), cppjson::is_array);
	BOOST_REQUIRE_EQUAL(i["constructors"].array().size(), 2);
	BOOST_CHECK_EQUAL(i["constructors"][0]["type"], "constructor");

	BOOST_CHECK_EQUAL(i["constructors"][0]["params"].type(), cppjson::is_array);
	BOOST_REQUIRE_EQUAL(i["constructors"][0]["params"].array().size(), 0);

	BOOST_REQUIRE_EQUAL(i["constructors"][1]["params"].type(), cppjson::is_array);
	BOOST_REQUIRE_EQUAL(i["constructors"][1]["params"].array().size(), 1);
	BOOST_CHECK_EQUAL(i["constructors"][1]["params"][0]["type"], "func_param");
	BOOST_CHECK_EQUAL(i["constructors"][1]["params"][0]["name"], "name");
}

BOOST_AUTO_TEST_CASE(aspect)
{
	auto pf = mi::parse("module mod v1.0: interface i { type name() mutable ; constructor(); constructor(type name); }");
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].content.size(), 1);

	cppjson::value result_ = mg::to_json(std::make_unique<test_json_asp>())(pf.mods);
	cppjson::value result = result_["mods"];

	BOOST_CHECK_EQUAL(result[0]["name"], "mod"s);
	BOOST_CHECK_EQUAL(result[0]["mn"], "mod"s);
}
