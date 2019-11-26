/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE utils_remove_empty

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parse.hpp"
#include "grammar/all.hpp"
#include "utils/remove_empty.hpp"

namespace ast = ix3::ast;
namespace txt = ix3::text;
namespace udt = boost::unit_test::data;
using namespace std::literals;
using ix3::utils::remove_empty;

BOOST_AUTO_TEST_CASE(normal_remove)
{
	ast::file_content result;
	std::string data = "module m1 v1.1: type foo(); module m2 v2.2: module m3 v3.3:";
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::file_content, data) );
	BOOST_CHECK_NO_THROW( result.modules | remove_empty{} );
	BOOST_REQUIRE_EQUAL( result.modules.size(), 1 );
	BOOST_TEST( result.modules[0].name == "m1"s );
	bool first_param_is_function = result.modules[0].content[0].var.type() == typeid(ast::function);
	BOOST_TEST( first_param_is_function );
}

BOOST_AUTO_TEST_CASE(no_remove)
{
	ast::file_content result;
	std::string data = "module m1 v1.1: type foo(); module m2 v2.2: type bar();";
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::file_content, data) );
	BOOST_CHECK_EQUAL( result.modules.size(), 2 );
	BOOST_CHECK_NO_THROW( result.modules | remove_empty{} );
	BOOST_REQUIRE_EQUAL( result.modules.size(), 2 );
}
