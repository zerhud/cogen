/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE to_generic_ast

#include <boost/test/unit_test.hpp>

#include "parse.hpp"
#include "grammar/all.hpp"
#include "utils/to_generic_ast.hpp"

namespace ast = ix3::ast;
namespace txt = ix3::text;
using namespace std::literals;
using ix3::utils::to_generic_ast;

BOOST_AUTO_TEST_SUITE(ix3)
BOOST_AUTO_TEST_SUITE(utils)
BOOST_AUTO_TEST_SUITE(gain_to_generic_ast)
BOOST_AUTO_TEST_CASE(empty_module)
{
	auto ast = txt::parse(txt::file_content, "module mod1 v1.1:"sv);
	//to_generic_ast maker;
	//maker(ast.modules);
}
BOOST_AUTO_TEST_SUITE_END() // gain_to_generic_ast
BOOST_AUTO_TEST_SUITE_END() // utils
BOOST_AUTO_TEST_SUITE_END() // ix3
