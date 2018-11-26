#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE modegen_parser

#include <boost/test/unit_test.hpp>

#include "split_by_version.hpp"
#include "grammar.hpp"

using namespace std::literals;

modegen::meta_parameters::version get_ver(const modegen::module& mod)
{
	using modegen::meta_parameters::version;
	for(auto& par:mod.meta_params) if(std::holds_alternative<version>(par)) return std::get<version>(par);
	assert(false);
	throw std::runtime_error("module must to have version");
}

BOOST_AUTO_TEST_CASE(functions)
{
	auto mods = modegen::parse("module mod v1.0: type name(); @v2.0 type name2();"sv);
	BOOST_REQUIRE_EQUAL(mods.size(), 1);

	modegen::split_by_version sp;
	sp(mods);

	BOOST_REQUIRE_EQUAL(mods.size(), 2);
	BOOST_REQUIRE_EQUAL(mods[0].content.size(), 1);
	BOOST_REQUIRE_EQUAL(mods[1].content.size(), 2);
	BOOST_REQUIRE_EQUAL(get_ver(mods[0]), modegen::meta_parameters::version(1,0));
	BOOST_REQUIRE_EQUAL(get_ver(mods[1]), modegen::meta_parameters::version(2,0));

	BOOST_CHECK_EQUAL(std::get<modegen::function>(mods[0].content[0]).name, "name");
	BOOST_CHECK_EQUAL(std::get<modegen::function>(mods[1].content[0]).name, "name");
	BOOST_CHECK_EQUAL(std::get<modegen::function>(mods[1].content[1]).name, "name2");

	using modegen::meta_parameters::version;
	BOOST_CHECK( !modegen::has<version>(mods[0].content[0]) );
	BOOST_CHECK( !modegen::has<version>(mods[1].content[0]) );
	BOOST_CHECK(  modegen::has<version>(mods[1].content[1]) );
}

BOOST_AUTO_TEST_CASE(interfaces)
{
	using modegen::meta_parameters::version;

	auto mods = modegen::parse("module mod v1.0: @v1.1 interface r1 {type func1() const; @v1.2 type func2() const; constructor(type a); @v1.3 constructor();}"sv);
	BOOST_REQUIRE_EQUAL(mods.size(), 1);

	auto& i0 = std::get<modegen::interface>(mods[0].content[0]);
	BOOST_CHECK_EQUAL(i0.constructors.size(), 2);

	modegen::split_by_version sp;
	sp(mods);
	BOOST_REQUIRE_EQUAL(mods.size(), 4);
	BOOST_REQUIRE_EQUAL(mods[0].content.size(), 0);
	BOOST_REQUIRE_EQUAL(mods[1].content.size(), 1);
	BOOST_REQUIRE_EQUAL(mods[2].content.size(), 1);
	BOOST_REQUIRE_EQUAL(mods[3].content.size(), 1);

	auto& i1 = std::get<modegen::interface>(mods[1].content[0]);
	BOOST_REQUIRE_EQUAL(i1.mem_funcs.size(), 1);
	BOOST_CHECK_EQUAL(i1.mem_funcs[0].name, "func1");
	BOOST_CHECK_EQUAL(i1.constructors.size(), 1);

	auto& i2 = std::get<modegen::interface>(mods[2].content[0]);
	BOOST_REQUIRE_EQUAL(i2.mem_funcs.size(), 2);
	BOOST_CHECK_EQUAL(i2.mem_funcs[1].name, "func2");
	BOOST_REQUIRE_EQUAL(i1.constructors.size(), 1);
	BOOST_CHECK_EQUAL(i1.constructors[0].func_params.size(), 1);

	auto& i3 = std::get<modegen::interface>(mods[3].content[0]);
	BOOST_CHECK(modegen::has<version>(mods[3]));
	BOOST_CHECK_EQUAL(*modegen::get<version>(mods[3]), version(1, 3));
	BOOST_REQUIRE_EQUAL(i3.mem_funcs.size(), 2);
	BOOST_CHECK_EQUAL(i3.mem_funcs[1].name, "func2");
	BOOST_REQUIRE_EQUAL(i3.constructors.size(), 2);
	BOOST_CHECK_EQUAL(i3.constructors[0].func_params.size(), 1);
	BOOST_CHECK_EQUAL(i3.constructors[1].func_params.size(), 0);
}
