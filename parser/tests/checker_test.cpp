#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE modegen_parser

#include <boost/test/unit_test.hpp>

#include "check.h"

using namespace std::literals;

void check_exception(const modegen::checker& ch, std::vector<modegen::module>& mods, std::string_view path)
{
	auto ech = [&path](const modegen::error_info& i){
		BOOST_CHECK_EQUAL( i.path, path );
		return i.path==path;
	};

	BOOST_CHECK_EXCEPTION(ch(mods, "test"sv), modegen::error_info, ech);
}

BOOST_AUTO_TEST_SUITE(double_name)
BOOST_AUTO_TEST_CASE(module)
{
	auto mods = modegen::parse("module mod v1: type name(); module mod2 v1: type name(); type name();"sv);
	BOOST_REQUIRE_EQUAL(mods.size(), 2);
	modegen::checker ch;
	check_exception(ch, mods, "mod2");
}
BOOST_AUTO_TEST_CASE(interface)
{
	auto mods = modegen::parse("module mod v1: interface i1{type name() const;} interface i2{type name() const; type name() const;}"sv);
	BOOST_REQUIRE_EQUAL(mods.size(), 1);
	modegen::checker ch;
	check_exception(ch, mods, "mod.i2");
}
BOOST_AUTO_TEST_SUITE_END()
