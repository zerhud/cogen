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
#include "utils/details/ix3_node_base.hpp"
#include "common_utils/tests/mocks.hpp"

namespace ast = ix3::ast;
namespace txt = ix3::text;
using namespace std::literals;
using ix3::utils::to_generic_ast;

BOOST_AUTO_TEST_SUITE(ix3)
BOOST_AUTO_TEST_SUITE(utils)
BOOST_AUTO_TEST_SUITE(gain_to_generic_ast)
boost::json::value make_json(const gen_utils::data_node& root, const gen_utils::tree& cnt)
{
	assert(dynamic_cast<const ix3::utils::details::ix3_node_base*>(&root));
	ix3::utils::details::compilation_context ctx(&cnt);
	return static_cast<const ix3::utils::details::ix3_node_base&>(root).make_json(ctx);
}
BOOST_AUTO_TEST_CASE(json_compare)
{
	BOOST_TEST(
		boost::json::parse(R"({"a":1,"b":2})"sv) ==
		boost::json::parse(R"({"b":2,"a":1})"sv)
		);
}
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
	BOOST_TEST(mod->node_var().value().name == "mod");
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

	BOOST_TEST(tree.to_json() == boost::json::parse(
	               R"({"name":"ix3","mods":[ {"name":"mod1","content":[
	                 {"type":"version","value":"1.1","content":[]},
	                 {"type":"version","value":"1.2","content":[]}
	               ]} ]})"sv));
}
BOOST_AUTO_TEST_CASE(records)
{
	to_generic_ast maker;
	auto ast = txt::parse(txt::file_content,
	                      "module mod1 v1.1:"
	                      "@v1.1 record rec { -int f1; @v1.2 +int f2;}"sv);
	gen_utils::tree tree = maker(ast.modules);
	auto mod = tree.children(*tree.children(tree.root()).at(0)).at(0);
	BOOST_TEST(tree.children(*mod).size()==1);

	auto rec = tree.children(*mod).at(0);
	BOOST_TEST(rec->name()=="rec");
	BOOST_CHECK(rec->version().value() == mod->version().value());

	auto rec_fields = tree.children(*rec);
	BOOST_TEST(rec_fields.size() == 2);
	BOOST_TEST(rec_fields.at(0)->name() == "f1");
	BOOST_TEST(mod->version().value() < rec_fields.at(1)->version().value());

	BOOST_TEST(make_json(*rec, tree) == boost::json::parse(
	               R"({
	               "name":"rec",
	               "type":"record",
	               "is_exception":false,
	               "fields":[
	                 {"name":"f1","type":"record_item","req":false,"param_t":
	                   {"type":"type","name":["int"],"subs":[]}},
	                 {"name":"f2","type":"record_item","req":true,"param_t":
	                   {"type":"type","name":["int"],"subs":[]}}
	               ]})"sv));
}
BOOST_AUTO_TEST_CASE(functions)
{
	to_generic_ast maker;
	auto ast = txt::parse(txt::file_content,
	                      "module mod1 v1.1:"
	                      "int foo(-string bar, +list<string> baz);"
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

	BOOST_TEST(make_json(*bar, tree) == boost::json::parse(
	               R"({
	               "name":"bar",
	               "type":"function",
	               "params":[],
	               "return":{"type":"type", "name":["int"], "subs":[]}
	               })"sv));
	BOOST_TEST(make_json(*foo_params.at(0), tree) == boost::json::parse(
	               R"({
	                 "name":"bar",
	                 "param_t":{"type":"type", "name":["string"], "subs":[]},
	                 "type":"function_parameter",
	                 "req":false
	               })"sv));
	BOOST_TEST(make_json(*foo_params.at(1), tree) == boost::json::parse(
	               R"({
	                 "name":"baz",
	                 "param_t":{"type":"type", "name":["list"], "subs":[
	                    {"type":"type", "name":["string"], "subs":[]}
	                 ]},
	                 "type":"function_parameter",
	                 "req":true
	               })"sv));
}
BOOST_AUTO_TEST_SUITE_END() // gain_to_generic_ast
BOOST_AUTO_TEST_SUITE_END() // utils
BOOST_AUTO_TEST_SUITE_END() // ix3
