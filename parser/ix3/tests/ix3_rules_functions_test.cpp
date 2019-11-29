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
#include "grammar/functions.hpp"
#include "operators/functions.hpp"

using namespace std::literals;
namespace ast = ix3::ast;
namespace txt = ix3::text;

BOOST_AUTO_TEST_CASE(function)
{
	ast::function result;

	std::string data = "static rtype foo(+type a) mutable"s;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::function, data) );
	BOOST_TEST( result.is_static.has_value() );
	BOOST_TEST( result.is_mutable.has_value() );
	BOOST_TEST( *result.is_static );
	BOOST_TEST( *result.is_mutable );
	BOOST_TEST( result.name == "foo"s );
	BOOST_TEST( result.params.size() == 1 );

	data = "static rtype foo() mutable"s;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::function, data) );
	BOOST_TEST( result.params.size() == 0 );

	data = "rtype foo()"s;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::function, data) );
	BOOST_TEST( result.params.size() == 0 );
	BOOST_TEST( !result.is_static.has_value() );
	BOOST_TEST( !result.is_mutable.has_value() );

	data = "rtype foo(+type a, -type b)"s;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::function, data) );
	BOOST_TEST( result.params.size() == 2 );
	BOOST_TEST( result.params[0].name == "a"s );
	BOOST_TEST( result.params[0].param_type.name.at(0) == "type"s );
	BOOST_TEST( result.params[0].required == true );
	BOOST_TEST( result.params[1].required == false );
	BOOST_TEST( result.return_type.name.at(0) == "rtype"s );
}

