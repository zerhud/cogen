/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE utils_naming

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "common_utils/naming.hpp"

using namespace gen_utils;

void check_some_name(const std::vector<std::string>& ns)
{
	BOOST_REQUIRE_EQUAL(ns.size(), 2);
	BOOST_CHECK_EQUAL(ns[0], "some");
	BOOST_CHECK_EQUAL(ns[1], "name");
}

BOOST_AUTO_TEST_SUITE(split)
BOOST_AUTO_TEST_CASE(one)
{
	auto ns = split_name("some");
	BOOST_REQUIRE_EQUAL(ns.size(), 1);
	BOOST_CHECK_EQUAL(ns[0], "some");
}
BOOST_AUTO_TEST_CASE(underscore)
{
	auto ns = split_name("some_name");
	check_some_name(ns);
	ns = split_name("some_Name");
	check_some_name(ns);
	ns = split_name("soMe_nAme");
	check_some_name(ns);
}
BOOST_AUTO_TEST_CASE(camel)
{
	auto ns = split_name("someName");
	check_some_name(ns);
}
BOOST_AUTO_TEST_CASE(title)
{
	auto ns = split_name("SomeName");
	check_some_name(ns);
}
BOOST_AUTO_TEST_CASE(with_num)
{
	auto ns = split_name("some1");
	BOOST_REQUIRE_EQUAL(ns.size(), 1);
	BOOST_CHECK_EQUAL(ns[0], "some1");

	ns = split_name("some1_name");
	BOOST_REQUIRE_EQUAL(ns.size(), 2);
	BOOST_CHECK_EQUAL(ns[0], "some1");
	BOOST_CHECK_EQUAL(ns[1], "name");
}
BOOST_AUTO_TEST_SUITE_END() // split

BOOST_AUTO_TEST_SUITE(convert)

BOOST_AUTO_TEST_CASE(one)
{
	BOOST_CHECK_EQUAL("foo", gen_utils::convert("foo", name_conversion::underscore));
	BOOST_CHECK_EQUAL("foo", gen_utils::convert("foo", name_conversion::camel_case));
	BOOST_CHECK_EQUAL("Foo", gen_utils::convert("foo", name_conversion::title_case));
	BOOST_CHECK_EQUAL("foo1", gen_utils::convert("foo1", name_conversion::underscore));
	BOOST_CHECK_EQUAL("foo1", gen_utils::convert("foo1", name_conversion::camel_case));
	BOOST_CHECK_EQUAL("Foo1", gen_utils::convert("foo1", name_conversion::title_case));
}
BOOST_AUTO_TEST_CASE(underscore)
{
	BOOST_CHECK_EQUAL("foo_bar", gen_utils::convert("fooBar", name_conversion::underscore));
	BOOST_CHECK_EQUAL("foo_bar", gen_utils::convert("FooBar", name_conversion::underscore));
	BOOST_CHECK_EQUAL("foo_bar", gen_utils::convert("Foo_Bar", name_conversion::underscore));
}
BOOST_AUTO_TEST_CASE(camel)
{
	BOOST_CHECK_EQUAL("fooBar", gen_utils::convert("fooBar", name_conversion::camel_case));
	BOOST_CHECK_EQUAL("fooBar", gen_utils::convert("FooBar", name_conversion::camel_case));
	BOOST_CHECK_EQUAL("fooBar", gen_utils::convert("Foo_Bar", name_conversion::camel_case));
}
BOOST_AUTO_TEST_CASE(title)
{
	BOOST_CHECK_EQUAL("FooBar", gen_utils::convert("fooBar", name_conversion::title_case));
	BOOST_CHECK_EQUAL("FooBar", gen_utils::convert("FooBar", name_conversion::title_case));
	BOOST_CHECK_EQUAL("FooBar", gen_utils::convert("Foo_Bar", name_conversion::title_case));
}
BOOST_AUTO_TEST_CASE(as_is)
{
	BOOST_CHECK_EQUAL("FooBar", gen_utils::convert("FooBar", name_conversion::as_is));
	BOOST_CHECK_EQUAL("FooBar_", gen_utils::convert("FooBar_", name_conversion::as_is));
	BOOST_CHECK_EQUAL("Foo_Bar", gen_utils::convert("Foo_Bar", name_conversion::as_is));
}
BOOST_AUTO_TEST_SUITE_END() // convert
