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

#include "parser/interface/grammar.hpp"
#include "parser/interface/helpers.hpp"

using namespace std::literals;

BOOST_AUTO_TEST_CASE(empty_module)
{
	std::vector<std::string_view> mods = {
	    {"module mod v1.0:"sv}
	  , {"#docs\nmodule mod v1.0:"sv}
	  , {"#docs\n@v3.0 module mod v1.0:"sv}
	};

	for(auto mod:mods) BOOST_CHECK_EQUAL(modegen::parser::interface::parse(mod).mods.size(), 1);
}

BOOST_AUTO_TEST_CASE(wrong_module)
{
	std::vector<std::string_view> mods = {
	    {"module mod:"sv} // no version
	  , {"module\nmod v1.0:"sv} // new line after module keyword
	  , {"module mod v1.0"sv} // no : after definition
	  , {"#docs\n@v3.0 module mod v1.0"sv} // extra version argument
	};

	for(auto mod:mods) {
		BOOST_CHECK_THROW(modegen::parser::interface::parse(mod), std::runtime_error);
	}

	// no new line after documentation (no module difinition)
	auto no_mod = modegen::parser::interface::parse("#docs module mod v1.0:"sv);
	BOOST_CHECK_EQUAL(no_mod.mods.size(), 0);
}

BOOST_AUTO_TEST_CASE(module_params)
{
	auto pf = modegen::parser::interface::parse("#docs\nmodule mod v1.0:");
	BOOST_REQUIRE_EQUAL(pf.mods[0].meta_params.set.size(), 2);
	BOOST_REQUIRE_EQUAL(pf.mods[0].meta_params.set[0].index(), 1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].meta_params.set[1].index(), 0);
}

BOOST_AUTO_TEST_CASE(using_rule)
{
	auto pf = modegen::parser::interface::parse("module mod v1.0: using some"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].imports.size(), 1);
	BOOST_CHECK_EQUAL(pf.mods[0].imports[0].mod_name, "some" );

	pf = modegen::parser::interface::parse("module mod v1.0: using some   "sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 1);

	pf = modegen::parser::interface::parse("module mod v1.0: using some\n\n\n"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 1);
}

BOOST_AUTO_TEST_CASE(record_rule)
{
	auto pf = modegen::parser::interface::parse("module mod v1.0: record some{type some; @v4.0 type other;}"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].content.size(),1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].content[0].index(),2);

	modegen::parser::interface::record& rec = std::get<modegen::parser::interface::record>(pf.mods[0].content[0]);
	BOOST_REQUIRE_EQUAL(rec.name, "some");
	BOOST_REQUIRE_EQUAL(rec.members.size(), 2);
}

BOOST_AUTO_TEST_CASE(interfaceq_rule)
{
	auto pf = modegen::parser::interface::parse("module mod v1.0: interface inter{ constructor(); static type some() const; type other() mutable;}"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(), 1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].content.size(),1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].content[0].index(),3);

	modegen::parser::interface::interface& i = std::get<modegen::parser::interface::interface>(pf.mods[0].content[0]);
	BOOST_CHECK_EQUAL(i.name, "inter");
	BOOST_CHECK_EQUAL(i.realization_in_client, false);

	BOOST_REQUIRE_EQUAL(i.mem_funcs.size(), 2);
	BOOST_CHECK_EQUAL(i.mem_funcs[1].is_static.has_value(),false);
	BOOST_REQUIRE_EQUAL(i.mem_funcs[0].is_static.has_value(),true);
	BOOST_CHECK_EQUAL(*i.mem_funcs[0].is_static,true);

	BOOST_REQUIRE_EQUAL(i.constructors.size(), 1);
	BOOST_REQUIRE_EQUAL(i.constructors[0].func_params.size(), 0);
}

BOOST_AUTO_TEST_SUITE(functions)
BOOST_AUTO_TEST_CASE(simple)
{
	std::vector<std::string_view> mods = {
	    {"module mod v1.0: type name();"sv}
	  , {"#docs\nmodule mod v1.0: type other();"sv}
	  , {"#docs\n@v3.0 module mod v1.0: type other();"sv}
	  , {"module mod v1.0:\ntype name();"sv}
	  , {"module mod v1.0:\ntype\nname();"sv}
	  , {"module mod v1.0:\ntype\nname\n(\n)\n;"sv}
	};

	for(auto mod:mods) BOOST_CHECK_EQUAL(modegen::parser::interface::parse(mod).mods.size(), 1);
}
BOOST_AUTO_TEST_CASE(no_params)
{
	auto with_n = "module mod v1.0:\ntype\nname\n(\n)\n;"sv;
	auto pf = modegen::parser::interface::parse(with_n);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].content.size(),1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].content[0].index(),0);
	BOOST_CHECK_EQUAL(std::get<modegen::parser::interface::function>(pf.mods[0].content[0]).func_params.size(), 0);
}
BOOST_AUTO_TEST_CASE(with_mod)
{
	std::vector<std::string_view> with_n = {
	    "module mod v1.0:list<type> name();"sv,
	    "module mod v1.0:list <type> name();"sv,
	    "module mod v1.0:list < type > name();"sv
//	    "module mod v1.0:list\n<\ntype\n>\nname();"sv
	};

	auto checker = [](std::string_view str) {
		auto pf = modegen::parser::interface::parse(str);
		BOOST_REQUIRE_EQUAL(pf.mods.size(),1);
		BOOST_REQUIRE_EQUAL(pf.mods[0].content.size(),1);
		BOOST_REQUIRE_EQUAL(pf.mods[0].content[0].index(),0);

		modegen::parser::interface::function& fnc = std::get<modegen::parser::interface::function>(pf.mods[0].content[0]);
		BOOST_CHECK_EQUAL(fnc.func_params.size(), 0);
		BOOST_CHECK_EQUAL(fnc.return_type.name, "list");
		BOOST_REQUIRE_EQUAL(fnc.return_type.sub_types.size(),1);
		BOOST_CHECK_EQUAL(fnc.return_type.sub_types[0].name,"type");
	};

	for(auto& str:with_n) checker(str);

	auto pf = modegen::parser::interface::parse("module mod v1.0:map<string,i8> name();"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].content.size(),1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].content[0].index(),0);
	modegen::parser::interface::function& fnc = std::get<modegen::parser::interface::function>(pf.mods[0].content[0]);
	BOOST_CHECK_EQUAL(fnc.return_type.name, "map");
	BOOST_REQUIRE_EQUAL(fnc.return_type.sub_types.size(),2);
	BOOST_CHECK_EQUAL(fnc.return_type.sub_types[0].name, "string");
	BOOST_CHECK_EQUAL(fnc.return_type.sub_types[1].name, "i8");
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(enumerations)
BOOST_AUTO_TEST_CASE(simple)
{
	auto two_mems = "module mod v1.0: enum lala{one two}"sv;
	auto pf = modegen::parser::interface::parse(two_mems);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].content.size(),1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].content[0].index(),1);

	modegen::parser::interface::enumeration& e = std::get<modegen::parser::interface::enumeration>(pf.mods[0].content[0]);
	BOOST_CHECK_EQUAL(e.name, "lala");
	BOOST_CHECK_EQUAL(e.elements.size(), 2);
	for(auto& el:e.elements) BOOST_CHECK(el.io.empty());
	BOOST_CHECK_EQUAL(e.meta_params.set.size(), 0);
	BOOST_CHECK_EQUAL(e.gen_io, false);
	BOOST_CHECK_EQUAL(e.use_bitmask, false);
}
BOOST_AUTO_TEST_CASE(params)
{
	auto two_mems = "module mod v1.0: enum lala{one=>'one' two=>'two' }"sv;
	auto pf = modegen::parser::interface::parse(two_mems);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].content.size(),1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].content[0].index(),1);

	modegen::parser::interface::enumeration& e = std::get<modegen::parser::interface::enumeration>(pf.mods[0].content[0]);
	BOOST_CHECK_EQUAL(e.elements.size(), 2);
	BOOST_CHECK_EQUAL(e.elements[0].name, "one");
	BOOST_CHECK_EQUAL(e.elements[0].io, "one");
	BOOST_CHECK_EQUAL(e.elements[1].name, "two");
	BOOST_CHECK_EQUAL(e.elements[1].io, "two");
}
BOOST_AUTO_TEST_CASE(gen_ops)
{
	auto two_mems = "module mod v1.0: enum +flags +auto_io lala{one=>'oneio' two=>'twoio' }"sv;
	auto pf = modegen::parser::interface::parse(two_mems);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].content.size(),1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].content[0].index(),1);

	modegen::parser::interface::enumeration& e = std::get<modegen::parser::interface::enumeration>(pf.mods[0].content[0]);
	BOOST_REQUIRE_EQUAL(e.elements.size(), 2);
	BOOST_CHECK_EQUAL(e.elements[0].name, "one");
	BOOST_CHECK_EQUAL(e.elements[1].name, "two");
	BOOST_CHECK_EQUAL(e.elements[0].io, "oneio");
	BOOST_CHECK_EQUAL(e.elements[1].io, "twoio");
	BOOST_CHECK_EQUAL(e.gen_io, true);
	BOOST_CHECK_EQUAL(e.use_bitmask, true);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(depricated_meta)
{
	using modegen::parser::interface::meta_parameters::deprication;
	using modegen::parser::interface::meta_parameters::version;

	auto pf = modegen::parser::interface::parse("module mod v1.0: @depricated v1.1 ('message') type f1();"sv);
	BOOST_REQUIRE_EQUAL(pf.mods.size(),1);
	BOOST_REQUIRE_EQUAL(pf.mods[0].content.size(),1);

	modegen::parser::interface::function& f = std::get<modegen::parser::interface::function>(pf.mods[0].content[0]);
	BOOST_REQUIRE_EQUAL(f.meta_params.set.size(), 1);

	BOOST_REQUIRE(std::holds_alternative<deprication>(f.meta_params.set[0]));
	deprication& dep = std::get<deprication>(f.meta_params.set[0]);
	BOOST_CHECK_EQUAL(dep.message, "message");
	BOOST_CHECK_EQUAL(dep.since, version(1,1));
}

