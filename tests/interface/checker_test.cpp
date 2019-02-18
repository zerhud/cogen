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

#include "errors.h"
#include "parser/interface/check.hpp"
#include "parser/interface/helpers.hpp"

using namespace std::literals;
namespace mi = modegen::parser::interface;

void check_exception(mi::checker& ch, mi::parsed_file& fi, std::string_view path)
{
	auto ech = [&path](const modegen::errors::error_info& i){
		BOOST_CHECK_EQUAL( i.path, path );
		return i.path==path;
	};

	fi.path = "test";
	BOOST_CHECK_EXCEPTION(ch(fi), modegen::errors::error_info, ech);
}

BOOST_AUTO_TEST_SUITE(double_name)
BOOST_AUTO_TEST_CASE(module)
{
	auto pf = mi::parse("module mod v1.0: type name(); module mod2 v1.0: type name(); type name();"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 2);
	mi::checker ch;
	check_exception(ch, pf, "mod2");
	BOOST_CHECK_EQUAL(pf.mods[0].file_name, "test");
	BOOST_CHECK_EQUAL(pf.mods[1].file_name, "test");
}
BOOST_AUTO_TEST_CASE(interface)
{
	auto pf = mi::parse("module mod v1.0: interface i1{type name() const;} interface i2{type name() const; type name() const;}"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 1);
	mi::checker ch;
	check_exception(ch, pf, "mod.i2");
}
BOOST_AUTO_TEST_CASE(double_ver)
{
	auto pf = mi::parse("@v1.0 module mod v1.0: interface i1{type name() const;}"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 1);
	mi::checker ch;
	check_exception(ch, pf, "mod");
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(method_const_mark)
{
	auto pf = mi::parse("module mod v1.0: interface i1{type name() const;} interface i2{type name();}"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 1);
	mi::checker ch;
	check_exception(ch, pf, "mod.i2.name");
}

BOOST_AUTO_TEST_CASE(mod_ver_is_min)
{
	auto pf = mi::parse("module mod v10.0: @v9.0 type some(); module mod2 v10.0: interface i{@v9.0 type name()const;}"
	                    "module mod v9.0: @v10.1 interface i{@v10.0 type name()const;}"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 3);
	mi::checker ch;
	mi::parsed_file m1({pf.mods[0]}), m2({pf.mods[1]}), m3({pf.mods[2]});
	check_exception(ch, m1, "mod.some");
	check_exception(ch, m2, "mod2.i.name");
	check_exception(ch, m3, "mod.i.name");
}

BOOST_AUTO_TEST_CASE(enum_gen_io)
{
	auto pf = mi::parse("module mod v1.0: enum +auto_io some_enum {one two three=>'3'}"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].content.size(), 1);

	mi::enumeration& e = std::get<mi::enumeration>(pf.mods[0].content[0]);
	BOOST_CHECK(e.gen_io);
	BOOST_CHECK(e.elements[0].io.empty());

	mi::checker ch;
	BOOST_CHECK_NO_THROW(ch(pf));

	BOOST_CHECK_EQUAL(e.elements.size(), 3);
	BOOST_CHECK_EQUAL(e.elements[0].io, e.elements[0].name);
	BOOST_CHECK_EQUAL(e.elements[1].io, e.elements[1].name);
	BOOST_CHECK_EQUAL(e.elements[2].io, "3");
}

BOOST_AUTO_TEST_SUITE(combination)
BOOST_AUTO_TEST_CASE(diff_mods)
{
	auto pf = mi::parse("module mod v1.0: type name(); module mod2 v1.0:"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 2);

	mi::checker ch;
	ch(pf);
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 2);
	BOOST_CHECK_EQUAL(pf.mods[0].name, "mod");
	BOOST_CHECK_EQUAL(pf.mods[1].name, "mod2");
}
BOOST_AUTO_TEST_CASE(diff_mods_ver)
{
	auto pf = mi::parse("module mod v1.0: type name(); module mod v2.0:"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 2);

	mi::checker ch;
	ch(pf);
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 2);
	BOOST_CHECK_EQUAL(pf.mods[0].name, "mod");
	BOOST_CHECK_EQUAL(pf.mods[1].name, "mod");
}
BOOST_AUTO_TEST_CASE(combine)
{
	auto pf = mi::parse("module mod v1.0: module mod v1.0: type name();"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 2);

	mi::checker ch;
	auto mods = ch(pf).extract_result();
	BOOST_REQUIRE_EQUAL(mods.size(), 1);
	BOOST_CHECK_EQUAL(mods[0].name, "mod");
	BOOST_CHECK_EQUAL(mods[0].content.size(), 1);
}
BOOST_AUTO_TEST_CASE(combine3)
{
	auto pf = mi::parse("module mod v1.0: module mod v1.0: type name(); module mod v1.0: type name2();"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 3);

	mi::checker ch;
	auto mods = ch(pf).extract_result();
	BOOST_REQUIRE_EQUAL(mods.size(), 1);
	BOOST_CHECK_EQUAL(mods[0].name, "mod");
	BOOST_CHECK_EQUAL(mods[0].content.size(), 2);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(exports)
{
	auto pf = mi::parse("module mod v1.0: interface i{} record r{} enum e{} void f();"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 1);
	mi::checker ch;
	auto mods = ch(pf).extract_result();

	BOOST_REQUIRE_EQUAL(mods[0].exports.size(), 4);
	BOOST_CHECK_EQUAL(mods[0].exports[0].name, "i");
	BOOST_CHECK_EQUAL(mods[0].exports[0].type, mi::module_content_selector::interface);

	BOOST_CHECK_EQUAL(mods[0].exports[1].name, "r");
	BOOST_CHECK_EQUAL(mods[0].exports[1].type, mi::module_content_selector::record);

	BOOST_CHECK_EQUAL(mods[0].exports[2].name, "e");
	BOOST_CHECK_EQUAL(mods[0].exports[2].type, mi::module_content_selector::enumeration);

	BOOST_CHECK_EQUAL(mods[0].exports[3].name, "f");
	BOOST_CHECK_EQUAL(mods[0].exports[3].type, mi::module_content_selector::function);
}
