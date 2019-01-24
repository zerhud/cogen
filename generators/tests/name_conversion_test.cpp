#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE name_conversion

#include <boost/test/unit_test.hpp>

#include "converters/case.h"

void check_some_name(const std::vector<std::string>& ns)
{
	BOOST_REQUIRE_EQUAL(ns.size(), 2);
	BOOST_CHECK_EQUAL(ns[0], "some");
	BOOST_CHECK_EQUAL(ns[1], "name");
}

BOOST_AUTO_TEST_SUITE(split)
BOOST_AUTO_TEST_CASE(one)
{
	auto ns = modegen::cvt::naming::split_name("some");
	BOOST_REQUIRE_EQUAL(ns.size(), 1);
	BOOST_CHECK_EQUAL(ns[0], "some");
}
BOOST_AUTO_TEST_CASE(underscore)
{
	auto ns = modegen::cvt::naming::split_name("some_name");
	check_some_name(ns);
	ns = modegen::cvt::naming::split_name("some_Name");
	check_some_name(ns);
	ns = modegen::cvt::naming::split_name("soMe_nAme");
	check_some_name(ns);
}
BOOST_AUTO_TEST_CASE(camel)
{
	auto ns = modegen::cvt::naming::split_name("someName");
	check_some_name(ns);
}
BOOST_AUTO_TEST_CASE(title)
{
	auto ns = modegen::cvt::naming::split_name("SomeName");
	check_some_name(ns);
}
BOOST_AUTO_TEST_CASE(with_num)
{
	auto ns = modegen::cvt::naming::split_name("some1");
	BOOST_REQUIRE_EQUAL(ns.size(), 1);
	BOOST_CHECK_EQUAL(ns[0], "some1");

	ns = modegen::cvt::naming::split_name("some1_name");
	BOOST_REQUIRE_EQUAL(ns.size(), 2);
	BOOST_CHECK_EQUAL(ns[0], "some1");
	BOOST_CHECK_EQUAL(ns[1], "name");
}
BOOST_AUTO_TEST_SUITE_END() // split

BOOST_AUTO_TEST_SUITE(convert)
using modegen::cvt::name_conversion;
using modegen::cvt::naming;
BOOST_AUTO_TEST_CASE(one)
{
	BOOST_CHECK_EQUAL("foo", naming::convert("foo", name_conversion::underscore));
	BOOST_CHECK_EQUAL("foo", naming::convert("foo", name_conversion::camel_case));
	BOOST_CHECK_EQUAL("Foo", naming::convert("foo", name_conversion::title_case));
	BOOST_CHECK_EQUAL("foo1", naming::convert("foo1", name_conversion::underscore));
	BOOST_CHECK_EQUAL("foo1", naming::convert("foo1", name_conversion::camel_case));
	BOOST_CHECK_EQUAL("Foo1", naming::convert("foo1", name_conversion::title_case));
}
BOOST_AUTO_TEST_CASE(underscore)
{
	BOOST_CHECK_EQUAL("foo_bar", naming::convert("fooBar", name_conversion::underscore));
	BOOST_CHECK_EQUAL("foo_bar", naming::convert("FooBar", name_conversion::underscore));
	BOOST_CHECK_EQUAL("foo_bar", naming::convert("Foo_Bar", name_conversion::underscore));
}
BOOST_AUTO_TEST_CASE(camel)
{
	BOOST_CHECK_EQUAL("fooBar", naming::convert("fooBar", name_conversion::camel_case));
	BOOST_CHECK_EQUAL("fooBar", naming::convert("FooBar", name_conversion::camel_case));
	BOOST_CHECK_EQUAL("fooBar", naming::convert("Foo_Bar", name_conversion::camel_case));
}
BOOST_AUTO_TEST_CASE(title)
{
	BOOST_CHECK_EQUAL("FooBar", naming::convert("fooBar", name_conversion::title_case));
	BOOST_CHECK_EQUAL("FooBar", naming::convert("FooBar", name_conversion::title_case));
	BOOST_CHECK_EQUAL("FooBar", naming::convert("Foo_Bar", name_conversion::title_case));
}
BOOST_AUTO_TEST_SUITE_END() // convert
