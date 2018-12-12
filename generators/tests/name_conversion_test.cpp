#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE modegen_parser

#include <boost/test/unit_test.hpp>

#include "case_converter.h"

void check_some_name(const std::vector<std::string>& ns)
{
	BOOST_REQUIRE_EQUAL(ns.size(), 2);
	BOOST_CHECK_EQUAL(ns[0], "some");
	BOOST_CHECK_EQUAL(ns[1], "name");
}

BOOST_AUTO_TEST_SUITE(split)
BOOST_AUTO_TEST_CASE(one)
{
	auto ns = modegen::generators::split_name("some");
	BOOST_REQUIRE_EQUAL(ns.size(), 1);
	BOOST_CHECK_EQUAL(ns[0], "some");
}
BOOST_AUTO_TEST_CASE(underscore)
{
	auto ns = modegen::generators::split_name("some_name");
	check_some_name(ns);
	ns = modegen::generators::split_name("some_Name");
	check_some_name(ns);
	ns = modegen::generators::split_name("soMe_nAme");
	check_some_name(ns);
}
BOOST_AUTO_TEST_CASE(camel)
{
	auto ns = modegen::generators::split_name("someName");
	check_some_name(ns);
}
BOOST_AUTO_TEST_CASE(title)
{
	auto ns = modegen::generators::split_name("SomeName");
	check_some_name(ns);
}
BOOST_AUTO_TEST_SUITE_END() // split
