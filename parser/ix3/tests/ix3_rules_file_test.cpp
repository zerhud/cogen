/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE rules_file

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parse.hpp"
#include "grammar/file.hpp"
//#include "operators/file.hpp"

constexpr bool run_known_bugs = false;

using namespace std::literals;
namespace ast = ix3::ast;
namespace txt = ix3::text;
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(ix3)
BOOST_AUTO_TEST_SUITE(ast_tests)

BOOST_AUTO_TEST_CASE(include_st)
{
	ast::include_st result;

	std::string data = "include \"some\"";
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::include_st, data) );
}

BOOST_AUTO_TEST_CASE(module)
{
	ast::module result;

	std::string data = "module mod v2.0:";
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::module, data) );
	BOOST_TEST( result.name == "mod"s );
	BOOST_TEST( result.version.major_v == 2 );
	BOOST_TEST( result.version.minor_v == 0 );
	BOOST_TEST( result.content.size() == 0 );

	data = "module mod v2.0: enum e{a,b}";
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::module, data) );
	BOOST_TEST( result.content.size() == 1 );

	data = "module mod v2.0: enum e{a,b} type fnc();";
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::module, data) );
	BOOST_TEST( result.content.size() == 2 );
}

BOOST_AUTO_TEST_SUITE_END() // ast_tests
BOOST_AUTO_TEST_SUITE_END() // ix3
