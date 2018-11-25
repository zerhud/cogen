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
	auto mods = modegen::parse("module mod v1.0: type name(); module mod2 v1.0: type name(); type name();"sv);
	BOOST_REQUIRE_EQUAL(mods.size(), 2);
	modegen::checker ch;
	check_exception(ch, mods, "mod2");
	BOOST_CHECK_EQUAL(mods[0].file_name, "test");
	BOOST_CHECK_EQUAL(mods[1].file_name, "test");
}
BOOST_AUTO_TEST_CASE(interface)
{
	auto mods = modegen::parse("module mod v1.0: interface i1{type name() const;} interface i2{type name() const; type name() const;}"sv);
	BOOST_REQUIRE_EQUAL(mods.size(), 1);
	modegen::checker ch;
	check_exception(ch, mods, "mod.i2");
}
BOOST_AUTO_TEST_CASE(double_ver)
{
	auto mods = modegen::parse("@v1.0 module mod v1.0: interface i1{type name() const;}"sv);
	BOOST_REQUIRE_EQUAL(mods.size(), 1);
	modegen::checker ch;
	check_exception(ch, mods, "mod");
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(method_const_mark)
{
	auto mods = modegen::parse("module mod v1.0: interface i1{type name() const;} interface i2{type name();}"sv);
	BOOST_REQUIRE_EQUAL(mods.size(), 1);
	modegen::checker ch;
	check_exception(ch, mods, "mod.i2.name");
}

BOOST_AUTO_TEST_CASE(mod_ver_is_min)
{
	auto mods = modegen::parse("module mod v10.0: @v9.0 type some(); module mod2 v10.0: interface i{@v9.0 type name()const;}"
	                           "module mod v9.0: @v10.1 interface i{@v10.0 type name()const;}"sv);
	BOOST_REQUIRE_EQUAL(mods.size(), 3);
	modegen::checker ch;
	std::vector<modegen::module> m1{mods[0]}, m2{mods[1]}, m3{mods[2]};
	check_exception(ch, m1, "mod.some");
	check_exception(ch, m2, "mod2.i.name");
	check_exception(ch, m3, "mod.i.name");
}

BOOST_AUTO_TEST_CASE(enum_gen_io)
{
	auto mods = modegen::parse("module mod v1.0: enum +auto_io some_enum {one two three=>'3'}"sv);
	BOOST_REQUIRE_EQUAL(mods.size(), 1);
	BOOST_REQUIRE_EQUAL(mods[0].content.size(), 1);

	modegen::enumeration& e = std::get<modegen::enumeration>(mods[0].content[0]);
	BOOST_CHECK(e.gen_io);
	BOOST_CHECK(e.elements[0].io.empty());

	modegen::checker ch;
	BOOST_CHECK_NO_THROW(ch(mods));

	BOOST_CHECK_EQUAL(e.elements.size(), 3);
	BOOST_CHECK_EQUAL(e.elements[0].io, e.elements[0].name);
	BOOST_CHECK_EQUAL(e.elements[1].io, e.elements[1].name);
	BOOST_CHECK_EQUAL(e.elements[2].io, "3");
}

BOOST_AUTO_TEST_SUITE(combination)
BOOST_AUTO_TEST_CASE(diff_mods)
{
	auto mods = modegen::parse("module mod v1.0: type name(); module mod2 v1.0:"sv);
	BOOST_REQUIRE_EQUAL(mods.size(), 2);

	modegen::checker ch;
	ch(mods);
	BOOST_REQUIRE_EQUAL(mods.size(), 2);
	BOOST_CHECK_EQUAL(mods[0].name, "mod");
	BOOST_CHECK_EQUAL(mods[1].name, "mod2");
}
BOOST_AUTO_TEST_CASE(diff_mods_ver)
{
	auto mods = modegen::parse("module mod v1.0: type name(); module mod v2.0:"sv);
	BOOST_REQUIRE_EQUAL(mods.size(), 2);

	modegen::checker ch;
	ch(mods);
	BOOST_REQUIRE_EQUAL(mods.size(), 2);
	BOOST_CHECK_EQUAL(mods[0].name, "mod");
	BOOST_CHECK_EQUAL(mods[1].name, "mod");
}
BOOST_AUTO_TEST_CASE(combine)
{
	auto mods = modegen::parse("module mod v1.0: module mod v1.0: type name();"sv);
	BOOST_REQUIRE_EQUAL(mods.size(), 2);

	modegen::checker ch;
	ch(mods);
	BOOST_REQUIRE_EQUAL(mods.size(), 1);
	BOOST_CHECK_EQUAL(mods[0].name, "mod");
	BOOST_CHECK_EQUAL(mods[0].content.size(), 1);
}
BOOST_AUTO_TEST_CASE(combine3)
{
	auto mods = modegen::parse("module mod v1.0: module mod v1.0: type name(); module mod v1.0: type name2();"sv);
	BOOST_REQUIRE_EQUAL(mods.size(), 3);

	modegen::checker ch;
	ch(mods);
	BOOST_REQUIRE_EQUAL(mods.size(), 1);
	BOOST_CHECK_EQUAL(mods[0].name, "mod");
	BOOST_CHECK_EQUAL(mods[0].content.size(), 2);
}
BOOST_AUTO_TEST_SUITE_END()
