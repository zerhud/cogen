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

BOOST_AUTO_TEST_CASE(records)
{
	auto mods = modegen::parse("module mod v1.0: enum e1 {one two} @v1.1 enum e2 {three four}"sv);
	BOOST_REQUIRE_EQUAL(mods.size(), 1);

	modegen::split_by_version sp;
	sp(mods);
	BOOST_REQUIRE_EQUAL(mods.size(), 2);
}
