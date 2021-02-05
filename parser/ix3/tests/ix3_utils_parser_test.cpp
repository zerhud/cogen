/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE utils_parser

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parser.hpp"

BOOST_AUTO_TEST_SUITE(ix3)
BOOST_AUTO_TEST_SUITE(utils)
BOOST_AUTO_TEST_SUITE(parser)
BOOST_AUTO_TEST_CASE(loads_single)
{
	std::stringstream data;
	data << "module mod v1.1: i64 foo();";
	ix3::parser p;
	p.parse(data, "1");
	auto result = p.result();
	BOOST_TEST(result.size() == 0);
	p.finish_loads();
	result = p.result();
	BOOST_TEST(result.size() == 1);
	BOOST_TEST(result.at(0).name == "mod");
	BOOST_TEST(result.at(0).content.size() == 1);
}
BOOST_AUTO_TEST_CASE(once_for_file)
{
	std::stringstream data1, data2, data3;
	data1 << "module mod1 v1.1: i64 foo();";
	data2 << "module mod2 v1.1: i64 foo();";
	data3 << "wrong content, cannot be parsed";
	ix3::parser p;
	p.parse(data1, "1");
	p.parse(data2, "1");
	BOOST_CHECK_THROW(p.parse(data3, "1"), std::exception);
	p.finish_loads();
	auto result = p.result();
	BOOST_TEST(result.size() == 1);
	BOOST_TEST(result.at(0).name == "mod1");
	BOOST_TEST(result.at(0).content.size() == 1);
}
BOOST_AUTO_TEST_CASE(search_for_includes)
{
	std::stringstream data1;
	data1 << "include \"2\"\nmodule mod1 v1.1: i64 foo();";

	auto solver = [](const std::filesystem::path& req) {
		BOOST_TEST(req=="2");
		return std::filesystem::path("/tmp/ix3_test_input_file_2");
	};

	ix3::parser p(solver);
	BOOST_CHECK_EXCEPTION(p.parse(data1, "1"), std::exception, [](const std::exception& ex){
		BOOST_TEST(ex.what() == "file /tmp/ix3_test_input_file_2 doesn't exists");
		return true;
	});
}
BOOST_AUTO_TEST_SUITE_END() // parser
BOOST_AUTO_TEST_SUITE_END() // utils
BOOST_AUTO_TEST_SUITE_END() // ix3
