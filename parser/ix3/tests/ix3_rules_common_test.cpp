/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE rules_common

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "mocks.hpp"
#include "parse.hpp"
#include "grammar/common.hpp"
#include "operators/common.hpp"

using namespace std::literals;
namespace ast = ix3::ast;
namespace txt = ix3::text;

BOOST_AUTO_TEST_SUITE(ix3)
BOOST_AUTO_TEST_SUITE(ast_tests)

BOOST_AUTO_TEST_CASE(type)
{
	ast::type result;

	std::string data = "list<type>"s;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::type, data) );
	BOOST_TEST( result.name.size() == 1 );
	BOOST_TEST( result.name.at(0) == "list"s );
	BOOST_TEST( result.sub_types.size() == 1 );
	BOOST_TEST( result.sub_types[0].get().name.at(0) == "type"s );
	BOOST_TEST( result.sub_types[0].get().sub_types.size() == 0 );

	data = "list"s;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::type, data) );
	BOOST_TEST( result.name.at(0) == "list"s );
	BOOST_TEST( result.sub_types.size() == 0 );

	data = "li st"s;
	std::stringstream out;
	ix3_mocks::parser_env env;
	MOCK_EXPECT(env.out).returns(std::ref(out));
	MOCK_EXPECT(env.file_name).returns("");
	MOCK_EXPECT(env.on_err_msg).once().with("parse not finished");
	BOOST_CHECK_NO_THROW( txt::parse(txt::type, data, env) );

	data = "list<a,b>"s;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::type, data) );
	BOOST_TEST( result.name.at(0) == "list"s );
	BOOST_TEST( result.sub_types.size() == 2 );
}

BOOST_AUTO_TEST_SUITE_END() // ast_tests
BOOST_AUTO_TEST_SUITE_END() // ix3
