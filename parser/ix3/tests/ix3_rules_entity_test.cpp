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

using namespace std::literals;
namespace ast = ix3::ast;
namespace txt = ix3::text;

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

BOOST_AUTO_TEST_CASE(record)
{
	BOOST_FAIL("empty test");
}
