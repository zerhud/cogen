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
#include "utils/naming.hpp"

namespace ast = ix3::ast;
namespace txt = ix3::text;
namespace udt = boost::unit_test::data;
using namespace std::literals;
using ix3::utils::naming;

BOOST_AUTO_TEST_CASE(to_camel_case)
{
	std::string data = "module m_o_d v1.1: int some_fnc(+type some_param);"s;
	ast::file_content result;
	result = txt::parse(txt::file_content, data);
	result.modules | naming(gen_utils::name_conversion::camel_case);
	BOOST_TEST_REQUIRE( result.modules.size() == 1 );
	BOOST_TEST( result.modules[0].name == "mOD" );
	ast::function& fnc = boost::get<ast::function>(result.modules[0].content[0].var);
	BOOST_TEST( fnc.name == "someFnc" );
	BOOST_TEST( fnc.params[0].name == "someParam" );
}

