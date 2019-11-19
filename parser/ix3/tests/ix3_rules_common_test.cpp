/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE rules_common

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parse.hpp"
#include "grammar/common.hpp"
#include "operators/common.hpp"

using namespace std::literals;
namespace ast = ix3::ast;
namespace txt = ix3::text;

BOOST_AUTO_TEST_CASE(type)
{
	ast::type result;
	std::string data = "list<type>"s;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::type, data) );
	BOOST_TEST( result.name == "list"s );
	BOOST_TEST( result.sub_types.size() == 1 );
	BOOST_TEST( result.sub_types[0].get().name == "type"s );
	BOOST_TEST( result.sub_types[0].get().sub_types.size() == 0 );

	data = "list"s;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::type, data) );
	BOOST_TEST( result.name == "list"s );
	BOOST_TEST( result.sub_types.size() == 0 );

	data = "li st"s;
	BOOST_CHECK_THROW( result = txt::parse(txt::type, data), std::exception );

	data = "list<a,b>"s;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::type, data) );
	BOOST_TEST( result.name == "list"s );
	BOOST_TEST( result.sub_types.size() == 2 );
}
