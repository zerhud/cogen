/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE rules_functions

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parse.hpp"
#include "grammar/entity.hpp"
//#include "operators/entity.hpp"

constexpr bool run_known_bugs = false;

using namespace std::literals;
namespace ast = ix3::ast;
namespace txt = ix3::text;
namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(ix3)
BOOST_AUTO_TEST_SUITE(ast_tests)

BOOST_AUTO_TEST_CASE(enumeration)
{
	ast::enumeration result;

	std::string data = "enum e{one, two=>\"two\"}"s;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::enumeration, data) );
	BOOST_TEST( result.name == "e"s );
	BOOST_TEST( result.gen_io == false );
	BOOST_TEST( result.use_bitmask == false );
	BOOST_TEST( result.elements.size() == 2 );
	BOOST_TEST( result.elements[0].name == "one"s );
	BOOST_TEST( result.elements[0].io == ""s );
	BOOST_TEST( result.elements[1].name == "two"s );
	BOOST_TEST( result.elements[1].io == "two"s );

	data = "@v1.1 enum e +auto_io {one, two=>\"two\"}"s;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::enumeration, data) );
	BOOST_TEST( result.name == "e"s );
	BOOST_TEST( result.gen_io == true );
	BOOST_TEST( result.meta_params.cnt.size() == 1 );
}

BOOST_AUTO_TEST_CASE(record_item)
{
	ast::record_item result;

	std::string data = "+type a";
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::record_item, data) );
	BOOST_TEST( result.name == "a"s );
	BOOST_TEST( result.is_required );
}

BOOST_AUTO_TEST_CASE(record)
{
	ast::record result;

	std::string data = "record r { +type a; }"s;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::record, data) );
	BOOST_TEST( result.name == "r"s );
	BOOST_TEST( result.members.size() == 1 );
}

BOOST_AUTO_TEST_CASE(interface)
{
	ast::interface result;

	std::string data = "interface i +i +ex { type foo(); }"s;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::interface, data) );
	BOOST_TEST( result.name == "i"s );
	BOOST_TEST( result.use_as_exception );
	BOOST_TEST( result.realization_in_client );
	BOOST_TEST( result.mem_funcs.size() == 1 );
	BOOST_TEST( result.constructors.size() == 0 );
}


BOOST_AUTO_TEST_CASE(interface_diff_exi,
		* utf::description("+ex and +i can to be in different order. dosen't work for now")
		* utf::enable_if<run_known_bugs>()
		)
{
	//NOTE: it should to be set of different flags for
	//      in ast we can mix order in flags.
	ast::interface result;

	std::string data = "interface ii +ex +i { type foo(); }"s;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::interface, data) );
	BOOST_TEST( result.name == "i"s );
	BOOST_TEST( result.use_as_exception );
	BOOST_TEST( result.realization_in_client );
}

BOOST_AUTO_TEST_SUITE_END() // ast_tests
BOOST_AUTO_TEST_SUITE_END() // ix3
