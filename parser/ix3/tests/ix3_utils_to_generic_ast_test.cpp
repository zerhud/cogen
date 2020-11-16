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
BOOST_AUTO_TEST_CASE(empty_modules)
{
	to_generic_ast maker;
	auto ast = txt::parse(txt::file_content, "module mod1 v1.1: module mod1 v1.2:"sv);
	auto tree = maker(ast.modules);
	BOOST_TEST(tree.root().name() == "ix3_root"sv);
	BOOST_TEST(tree.root().version().value() == 0);
	BOOST_CHECK(!tree.root().node_var().has_value());

	BOOST_TEST(tree.children(tree.root()).size()==1);
	auto mod = tree.children(tree.root()).at(0);
	BOOST_CHECK(!mod->version().has_value());
	BOOST_TEST(mod->node_var().value().name == "name");
	BOOST_TEST(mod->node_var().value().value == "mod1");

	auto vers = tree.children(*mod);
	BOOST_TEST(vers.size()==2);
	BOOST_TEST(vers.at(0)->name()=="1.1");
	BOOST_TEST(vers.at(1)->name()=="1.2");
	BOOST_CHECK(vers.at(0)->version().has_value());
	BOOST_CHECK(vers.at(1)->version().has_value());
	BOOST_TEST(vers.at(0)->node_var().value().name == "ver");
	BOOST_TEST(vers.at(0)->node_var().value().value == "1.1");
	BOOST_TEST(vers.at(1)->node_var().value().name == "ver");
	BOOST_TEST(vers.at(1)->node_var().value().value == "1.2");
}
BOOST_AUTO_TEST_CASE(functions)
{
	to_generic_ast maker;
	auto ast = txt::parse(txt::file_content,
	                      "module mod1 v1.1:"
	                      "int foo(+string bar, +list<string> baz);"
	                      "@v1.2 int bar();"sv);
	gen_utils::tree tree = maker(ast.modules);
	BOOST_TEST(tree.children(tree.root()).size()==1);
	auto mod = tree.children(*tree.children(tree.root()).at(0)).at(0);
	BOOST_TEST(tree.children(*mod).size()==2);

	auto foo = tree.children(*mod).at(0);
	BOOST_TEST(foo->name()=="foo");
	BOOST_CHECK(!foo->version().has_value());

	auto bar = tree.children(*mod).at(1);
	BOOST_TEST(bar->name()=="bar");
	BOOST_CHECK(bar->version().has_value());
	BOOST_TEST(*mod->version() < *bar->version());

	auto foo_params = tree.children(*foo);
	BOOST_TEST(foo_params.size() == 2);
	BOOST_TEST(foo_params.at(0)->name() == "bar");
	BOOST_CHECK(!foo_params.at(0)->version().has_value());
}
BOOST_AUTO_TEST_SUITE_END() // gain_to_generic_ast
BOOST_AUTO_TEST_SUITE_END() // utils
BOOST_AUTO_TEST_SUITE_END() // ix3
