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

BOOST_AUTO_TEST_CASE(using_rule)
{
	auto mods = modegen::parse("module mod v1: using some"sv);
	BOOST_REQUIRE_EQUAL(mods.size(), 1);
	BOOST_REQUIRE_EQUAL(mods[0].imports.size(), 1);
	BOOST_CHECK_EQUAL(mods[0].imports[0].mod_name, "some" );

	mods = modegen::parse("module mod v1: using some   "sv);
	BOOST_REQUIRE_EQUAL(mods.size(), 1);

	mods = modegen::parse("module mod v1: using some\n\n\n"sv);
	BOOST_REQUIRE_EQUAL(mods.size(), 1);
}

BOOST_AUTO_TEST_CASE(record_rule)
{
	auto mods = modegen::parse("module mod v1: record some{type some; type other;}"sv);
	BOOST_REQUIRE_EQUAL(mods.size(), 1);
	BOOST_REQUIRE_EQUAL(mods[0].content.size(),1);
	BOOST_REQUIRE_EQUAL(mods[0].content[0].index(),2);

	modegen::record& rec = std::get<modegen::record>(mods[0].content[0]);
	BOOST_REQUIRE_EQUAL(rec.name, "some");
	BOOST_REQUIRE_EQUAL(rec.members.size(), 2);
}

BOOST_AUTO_TEST_CASE(interfaceq_rule)
{
	auto mods = modegen::parse("module mod v1: interface inter{ constructor(); static type some() const; type other() mutable;}");
	BOOST_REQUIRE_EQUAL(mods.size(), 1);
	BOOST_REQUIRE_EQUAL(mods[0].content.size(),1);
	BOOST_REQUIRE_EQUAL(mods[0].content[0].index(),3);

	modegen::interface& i = std::get<modegen::interface>(mods[0].content[0]);
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
BOOST_AUTO_TEST_CASE(with_mod)
{
	std::vector<std::string_view> with_n = {
	    "module mod v1:list<type> name();"sv,
	    "module mod v1:list <type> name();"sv,
	    "module mod v1:list < type > name();"sv
//	    "module mod v1:list\n<\ntype\n>\nname();"sv
	};

	auto checker = [](std::string_view str) {
		auto mods = modegen::parse(str);
		BOOST_REQUIRE_EQUAL(mods.size(),1);
		BOOST_REQUIRE_EQUAL(mods[0].content.size(),1);
		BOOST_REQUIRE_EQUAL(mods[0].content[0].index(),0);

		modegen::function& fnc = std::get<modegen::function>(mods[0].content[0]);
		BOOST_CHECK_EQUAL(fnc.func_params.size(), 0);
		BOOST_CHECK_EQUAL(fnc.return_type.modificator, "list");
	};

	for(auto& str:with_n) checker(str);
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
	for(auto& el:e.elements) BOOST_CHECK(el.io.empty());
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
	BOOST_CHECK_EQUAL(e.elements[0].name, "one");
	BOOST_CHECK_EQUAL(e.elements[0].io, "one");
	BOOST_CHECK_EQUAL(e.elements[1].name, "two");
	BOOST_CHECK_EQUAL(e.elements[1].io, "two");
}
BOOST_AUTO_TEST_CASE(gen_ops)
{
	auto two_mems = "module mod v1: enum +flags +auto_io lala{one=>'oneio' two=>'twoio' }"sv;
	auto mods = modegen::parse(two_mems);
	BOOST_REQUIRE_EQUAL(mods.size(),1);
	BOOST_REQUIRE_EQUAL(mods[0].content.size(),1);
	BOOST_REQUIRE_EQUAL(mods[0].content[0].index(),1);

	modegen::enumeration& e = std::get<modegen::enumeration>(mods[0].content[0]);
	BOOST_REQUIRE_EQUAL(e.elements.size(), 2);
	BOOST_CHECK_EQUAL(e.elements[0].name, "one");
	BOOST_CHECK_EQUAL(e.elements[1].name, "two");
	BOOST_CHECK_EQUAL(e.elements[0].io, "oneio");
	BOOST_CHECK_EQUAL(e.elements[1].io, "twoio");
	BOOST_CHECK_EQUAL(e.gen_io, true);
	BOOST_CHECK_EQUAL(e.use_bitmask, true);
}
BOOST_AUTO_TEST_SUITE_END()
