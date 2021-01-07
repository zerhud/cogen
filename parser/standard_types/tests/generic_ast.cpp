/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE standard_types_parser

#include <iomanip>
#include <boost/test/included/unit_test.hpp>
#include <common_utils/tests/mocks.hpp>

#include "loader.hpp"

BOOST_AUTO_TEST_SUITE(library)
using std_types::loader;
BOOST_AUTO_TEST_CASE(cpp_types)
{
	std::stringstream data;
	data << R"(i8 {
	cpp {
		file cinttypes
		sys true
		replace std::int8_t
	}
}
string {
	cpp {
		file string
		sys true
		replace std::pmr::string
	}
})";
	loader ldr;
	auto tt = ldr.load_types(data);
	BOOST_TEST(tt.data_id() == "stdtypes");
	BOOST_TEST_REQUIRE(tt.search({"i8"}).size() == 1);
	BOOST_TEST_REQUIRE(tt.search({"string"}).size() == 1);
	auto i8 = tt.search({"i8"})[0];
	auto string = tt.search({"string"})[0];

	BOOST_TEST(i8->required_links().size()==0);
	BOOST_TEST(string->required_links().size()==0);

	BOOST_TEST(i8->link_condition()=="cpp");
	BOOST_TEST(string->link_condition()=="cpp");

	BOOST_REQUIRE(i8->imports_modification().has_value());
	BOOST_REQUIRE(string->imports_modification().has_value());

	BOOST_CHECK(i8->imports_modification()->sys);
	BOOST_CHECK(string->imports_modification()->sys);
	BOOST_TEST(i8->imports_modification()->name == "cinttypes");
	BOOST_TEST(string->imports_modification()->name == "string");
}
BOOST_AUTO_TEST_SUITE_END() // library


