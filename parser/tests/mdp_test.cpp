#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE modegen_parser

#include <boost/test/unit_test.hpp>

#include "grammar.hpp"

using namespace std::literals;

BOOST_AUTO_TEST_CASE(empty_module)
{
	std::vector<std::string_view> mods = {
	    {"module mod v1:"sv}
	  , {"#docs\nmodule mod v1:"sv}
	  , {"#docs\n@v3 module mod v1:"sv}
	};

	for(auto mod:mods) BOOST_CHECK_EQUAL(modegen::parse(mod).size(), 1);
}

BOOST_AUTO_TEST_CASE(wrong_module)
{
	std::vector<std::string_view> mods = {
	    {"module mod:"sv}
	  , {"module\nmod v1:"sv}
	  , {"module mod v1"sv}
	  , {"#docs module mod v1:"sv}
	  , {"#docs\n@v3 module mod v1"sv}
	};

	for(auto mod:mods) BOOST_CHECK_THROW(modegen::parse(mod), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(module_params)
{
	auto mods = modegen::parse("#docs\nmodule mod v1:");
	BOOST_REQUIRE_EQUAL(mods[0].meta_params.set.size(), 2);
	BOOST_REQUIRE_EQUAL(mods[0].meta_params.set[0].index(), 1);
	BOOST_REQUIRE_EQUAL(mods[0].meta_params.set[1].index(), 0);
}

BOOST_AUTO_TEST_SUITE(functions)
BOOST_AUTO_TEST_CASE(simple)
{
	std::vector<std::string_view> mods = {
	    {"module mod v1: type name();"sv}
	  , {"#docs\nmodule mod v1: type other();"sv}
	  , {"#docs\n@v3 module mod v1: type other();"sv}
	  , {"module mod v1:\ntype name();"sv}
	  , {"module mod v1:\ntype\nname();"sv}
	  , {"module mod v1:\ntype\nname\n(\n)\n;"sv}
	};

	for(auto mod:mods) BOOST_CHECK_EQUAL(modegen::parse(mod).size(), 1);
}
BOOST_AUTO_TEST_CASE(no_params)
{
	auto with_n = "module mod v1:\ntype\nname\n(\n)\n;"sv;
	auto mods = modegen::parse(with_n);
	BOOST_REQUIRE_EQUAL(mods.size(),1);
	BOOST_REQUIRE_EQUAL(mods[0].content.size(),1);
	BOOST_REQUIRE_EQUAL(mods[0].content[0].index(),0);
	BOOST_CHECK_EQUAL(std::get<modegen::function>(mods[0].content[0]).func_params.size(), 0);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(enumerations)
BOOST_AUTO_TEST_CASE(simple)
{
	auto two_mems = "module mod v1: enum lala{one two}"sv;
	auto mods = modegen::parse(two_mems);
	BOOST_REQUIRE_EQUAL(mods.size(),1);
	BOOST_REQUIRE_EQUAL(mods[0].content.size(),1);
	BOOST_REQUIRE_EQUAL(mods[0].content[0].index(),1);

	modegen::enumeration& e = std::get<modegen::enumeration>(mods[0].content[0]);
	BOOST_CHECK_EQUAL(e.name, "lala");
	BOOST_CHECK_EQUAL(e.elements.size(), 2);
	BOOST_CHECK_EQUAL(e.ios.size(), 0);
	BOOST_CHECK_EQUAL(e.meta_params.set.size(), 0);
	BOOST_CHECK_EQUAL(e.gen_io, false);
	BOOST_CHECK_EQUAL(e.use_bitmask, false);
}
BOOST_AUTO_TEST_CASE(params)
{
	auto two_mems = "module mod v1: enum lala{one=>'one' two=>'two' }"sv;
	auto mods = modegen::parse(two_mems);
	BOOST_REQUIRE_EQUAL(mods.size(),1);
	BOOST_REQUIRE_EQUAL(mods[0].content.size(),1);
	BOOST_REQUIRE_EQUAL(mods[0].content[0].index(),1);

	modegen::enumeration& e = std::get<modegen::enumeration>(mods[0].content[0]);
	BOOST_CHECK_EQUAL(e.elements.size(), 2);
	BOOST_REQUIRE_EQUAL(e.ios.size(), 2);
	BOOST_CHECK_EQUAL(e.ios[0], "one");
	BOOST_CHECK_EQUAL(e.ios[1], "two");
}
BOOST_AUTO_TEST_CASE(gen_ops)
{
	auto two_mems = "module mod v1: enum +flags +auto_io lala{one=>'one' two=>'two' }"sv;
	auto mods = modegen::parse(two_mems);
	BOOST_REQUIRE_EQUAL(mods.size(),1);
	BOOST_REQUIRE_EQUAL(mods[0].content.size(),1);
	BOOST_REQUIRE_EQUAL(mods[0].content[0].index(),1);

	modegen::enumeration& e = std::get<modegen::enumeration>(mods[0].content[0]);
	BOOST_CHECK_EQUAL(e.elements.size(), 2);
	BOOST_REQUIRE_EQUAL(e.ios.size(), 2);
	BOOST_CHECK_EQUAL(e.ios[0], "one");
	BOOST_CHECK_EQUAL(e.ios[1], "two");
	BOOST_CHECK_EQUAL(e.gen_io, true);
	BOOST_CHECK_EQUAL(e.use_bitmask, true);
}
BOOST_AUTO_TEST_SUITE_END()
