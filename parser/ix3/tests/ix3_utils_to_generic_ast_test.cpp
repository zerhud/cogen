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
#include "utils/details/sep_nodes.hpp"
#include "utils/details/ast_nodes.hpp"
#include "common_utils/tests/mocks.hpp"
#include "input/imports_manager.hpp"
#include "input/input.hpp"

namespace ast = ix3::ast;
namespace txt = ix3::text;
using namespace std::literals;
using ix3::utils::to_generic_ast;
using gunc = gen_utils::name_conversion;

boost::json::value operator "" _bj(const char* d, std::size_t l)
{
	boost::json::parse_options opts{.allow_trailing_commas=true};
	return boost::json::parse(
	            boost::json::string_view(d,l),
	            boost::json::storage_ptr(),
	            opts);
}

BOOST_AUTO_TEST_SUITE(ix3)
BOOST_AUTO_TEST_SUITE(utils)
BOOST_AUTO_TEST_SUITE(gain_to_generic_ast)
struct fake_compiler : ix3::utils::details::ix3_compiler {

	fake_compiler(const gen_utils::compilation_config& c) : cfg(c) {}
	const gen_utils::compilation_config& cfg;

	const gen_utils::compilation_config& config() const override {return cfg; }

	void aspect(const ix3::utils::details::ix3_node_base&, boost::json::object&) const override {}
	void aspect(const ix3::utils::details::module_node&, boost::json::object&) const override {}
	void aspect(const ix3::utils::details::module_version_node&, boost::json::object&) const override {}
	void aspect(const ix3::utils::details::function_node&, boost::json::object&) const override {}
	void aspect(const ix3::utils::details::record_node&, boost::json::object&) const override {}
};

boost::json::value make_json(
		const gen_utils::data_node& root,
		const gen_utils::tree& cnt,
		std::optional<gen_utils::compilation_context> i_ctx=std::nullopt)
{
	assert(dynamic_cast<const ix3::utils::details::ix3_node_base*>(&root));
	gen_utils::compilation_context gu_ctx;
	if(i_ctx) gu_ctx = std::move(*i_ctx);
	else gu_ctx.cfg.naming = {gen_utils::name_conversion::as_is};
	fake_compiler fc{gu_ctx.cfg};
	ix3::utils::details::compilation_context ctx(&cnt, &fc, &gu_ctx);
	return static_cast<const ix3::utils::details::ix3_node_base&>(root).make_json(ctx);
}
void check_naming(
        const gen_utils::data_node& root,
        const gen_utils::tree& cnt,
        const std::vector<std::string> names)
{
	gen_utils::compilation_context ctx;
	ctx.cfg.naming = {gunc::title_case};
	BOOST_TEST(make_json(root, cnt, ctx).as_object()["name"] == names.at(0).c_str());
	ctx.cfg.naming = {gunc::camel_case};
	BOOST_TEST(make_json(root, cnt, ctx).as_object()["name"] == names.at(1).c_str());
	ctx.cfg.naming = {gunc::underscore};
	BOOST_TEST(make_json(root, cnt, ctx).as_object()["name"] == names.at(2).c_str());
	ctx.cfg.naming = {gunc::camel_case, gunc::underscore};
	BOOST_TEST(make_json(root, cnt, ctx)
	           .as_object()["name"].as_array()[0] == names.at(1).c_str());
	BOOST_TEST(make_json(root, cnt, ctx)
	           .as_object()["name"].as_array()[1] == names.at(2).c_str());
}
BOOST_AUTO_TEST_CASE(json_compare)
{
	BOOST_TEST(R"({"a":1,"b":2})"_bj == R"({"b":2,"a":1})"_bj);
}
BOOST_AUTO_TEST_CASE(empty_modules)
{
	to_generic_ast maker;
	auto ast = txt::parse(txt::file_content, "module mod1 v1.1: module mod1 v1.2:");
	auto tree = maker(ast.modules);
	BOOST_TEST(tree.root().name() == "ix3"sv);
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

	gen_utils::compilation_context ctx;
	BOOST_TEST(tree.to_json(ctx) == R"([{"name":"mod1", "orig_name":"mod1","vers":[
	                 {"type":"version","minor":1,"major":1,"name":"mod1_v1_1","content":[]},
	                 {"type":"version","minor":2,"major":1,"name":"mod1_v1_2","content":[]}
	               ]}])"_bj);
	ctx.cfg.naming.emplace_back(gen_utils::name_conversion::title_case);
	BOOST_TEST(tree.to_json(ctx) == R"([{"name":["mod1","Mod1"], "orig_name":"mod1","vers":[
	                 {"type":"version","minor":1,"major":1,"name":"mod1_v1_1","content":[]},
	                 {"type":"version","minor":2,"major":1,"name":"mod1_v1_2","content":[]}
	               ]}])"_bj);
	ctx.cfg.naming.clear();
	BOOST_TEST(tree.to_json(ctx) == R"([{"name":"mod1", "orig_name":"mod1","vers":[
	                 {"type":"version","minor":1,"major":1,"name":"mod1_v1_1","content":[]},
	                 {"type":"version","minor":2,"major":1,"name":"mod1_v1_2","content":[]}
	               ]}])"_bj);
}
BOOST_AUTO_TEST_CASE(records)
{
	to_generic_ast maker;
	auto ast = txt::parse(txt::file_content,
	                      "module mod1 v1.1:"
	                      "@v1.1 record bar_baz { -int f1; @v1.2 +int f2;}");
	gen_utils::tree tree = maker(ast.modules);
	auto mod = tree.children(*tree.children(tree.root()).at(0)).at(0);
	BOOST_TEST(tree.children(*mod).size()==1);

	auto rec = tree.children(*mod).at(0);
	BOOST_TEST(rec->name()=="bar_baz");
	BOOST_CHECK(rec->version().value() == mod->version().value());

	auto rec_fields = tree.children(*rec);
	BOOST_TEST(rec_fields.size() == 2);
	BOOST_TEST(rec_fields.at(0)->name() == "f1");
	BOOST_TEST(mod->version().value() < rec_fields.at(1)->version().value());

	BOOST_TEST(tree.children(*rec_fields[0]).size()==1);
	BOOST_TEST(tree.children(*rec_fields[1]).size()==1);
	BOOST_TEST(tree.children(*rec_fields[0])[0]->
	        required_links().at(0).at(0) == "int");

	BOOST_TEST(make_json(*rec, tree) == R"({
	               "orig_name":"bar_baz","name":"bar_baz",
	               "type":"record",
	               "is_exception":false,
	               "fields":[
	                 {"orig_name":"f1","name":"f1","type":"record_item","req":false,"param_t":
	                   {"type":"type","name":["int"],"subs":[]}},
	                 {"orig_name":"f2","name":"f2","type":"record_item","req":true,"param_t":
	                   {"type":"type","name":["int"],"subs":[]}}
	               ]})"_bj);
	check_naming(*rec, tree, {"BarBaz"s, "BarBaz"s, "bar_baz"s});
}
BOOST_AUTO_TEST_CASE(functions)
{
	using ix3::utils::details::ix3_node_base;

	to_generic_ast maker;
	auto ast = txt::parse(txt::file_content,
	                      "module mod1 v1.1:"
	                      "int foo(-string bar, +list<string> baz);"
	                      "@v1.2 int bar_baz(+u8 b);");
	gen_utils::tree tree = maker(ast.modules);
	BOOST_TEST(tree.children(tree.root()).size()==1);
	auto mod = tree.children(*tree.children(tree.root()).at(0)).at(0);
	BOOST_TEST(tree.children(*mod).size()==2);

	auto foo = tree.children(*mod).at(0);
	BOOST_TEST(foo->name()=="");
	BOOST_CHECK(!foo->version().has_value());
	BOOST_TEST(foo->required_links().size() == 0);
	BOOST_TEST(dynamic_cast<const ix3_node_base&>(*foo).inner_name() == "foo");

	auto bar = tree.children(*mod).at(1);
	BOOST_TEST(bar->name()=="");
	BOOST_CHECK(bar->version().has_value());
	BOOST_TEST(*mod->version() < *bar->version());
	BOOST_TEST(dynamic_cast<const ix3_node_base&>(*bar).inner_name() == "bar_baz");

	auto foo_params = tree.children(*foo);
	BOOST_TEST(foo_params.size() == 3);
	BOOST_TEST(foo_params.at(1)->name() == "bar");
	BOOST_CHECK(!foo_params.at(1)->version().has_value());

	BOOST_TEST(make_json(*bar, tree) == R"({
	               "orig_name":"bar_baz","name":"bar_baz",
	               "type":"function",
	               "params":[ {
	                 "orig_name":"b","name":"b",
	                 "param_t":{"type":"type", "name":["u8"], "subs":[]},
	                 "type":"function_parameter",
	                 "req":true
	               } ],
	               "return":{"type":"type", "name":["int"], "subs":[]}
	               })"_bj);
	BOOST_TEST(make_json(*foo_params.at(0), tree) == 
	               R"({ "type":"type","name":["int"],"subs":[] })"_bj);
	BOOST_TEST(make_json(*foo_params.at(1), tree) == R"({
	                 "orig_name":"bar","name":"bar",
	                 "param_t":{"type":"type", "name":["string"], "subs":[]},
	                 "type":"function_parameter",
	                 "req":false
	               })"_bj);
	BOOST_TEST(make_json(*foo_params.at(2), tree) == R"({
	                 "orig_name":"baz","name":"baz",
	                 "param_t":{"type":"type", "name":["list"], "subs":[
	                    {"type":"type", "name":["string"], "subs":[]}
	                 ]},
	                 "type":"function_parameter",
	                 "req":true
	               })"_bj);

	check_naming(*bar, tree, {"BarBaz"s, "barBaz"s, "bar_baz"s});
}
BOOST_AUTO_TEST_CASE(type_nodes)
{
	to_generic_ast maker;
	auto ast = txt::parse(txt::file_content,
	                      "module mod1 v1.1:"
	                      "int foo(-string<char,alloc> bar, +list<foo<str_t>> bar2);");
	gen_utils::tree tree = maker(ast.modules);
	auto extract_type_node = [&tree](auto& par, int at){
		auto ret = dynamic_cast<const ix3::utils::details::type_node*>(
		            tree.children(*par).at(at).get());
		BOOST_REQUIRE(ret != nullptr);
		return ret;
	};
	auto mod = tree.children(*tree.children(tree.root()).at(0)).at(0);
	auto foo = tree.children(*mod).at(0);
	auto ret_type = extract_type_node(foo, 0);
	auto bar1 = tree.children(*foo).at(1);
	auto bar1_type = extract_type_node(bar1, 0);
	auto bar2 = tree.children(*foo).at(2);
	auto bar2_type = extract_type_node(bar2, 0);

	BOOST_TEST(ret_type->type_name() == "int");
	BOOST_TEST(tree.children(*ret_type).size()==0);
	BOOST_TEST(tree.children(*bar1).size() == 1);
	BOOST_TEST(tree.children(*bar2).size() == 1);
	BOOST_TEST(bar1_type->type_name() == "string");
	BOOST_TEST(bar2_type->type_name() == "list");
	BOOST_TEST(tree.children(*bar1_type).size()==2);
	BOOST_TEST(extract_type_node(bar1_type, 0)->type_name()=="char");
	BOOST_TEST(extract_type_node(bar1_type, 1)->type_name()=="alloc");
	BOOST_TEST(tree.children(*bar2_type).size()==1);
	auto bar2_foo = extract_type_node(bar2_type, 0);
	BOOST_TEST(bar2_foo->type_name() == "foo");
	BOOST_TEST(extract_type_node(bar2_foo, 0)->type_name() == "str_t");

	gen_utils::compilation_context ctx;
	BOOST_TEST(make_json(*ret_type, tree, ctx) == 
	               R"( {"type":"type", "name":["int"], "subs":[]} )"_bj);
	ctx.cfg.naming.emplace_back(gen_utils::name_conversion::title_case);
	BOOST_TEST(make_json(*ret_type, tree, ctx) == 
	               R"( {"type":"type", "name":["int"], "subs":[]} )"_bj);
	BOOST_TEST(make_json(*bar1_type, tree) == 
	               R"( {"type":"type", "name":["string"], "subs":[
	               {"type":"type", "name":["char"], "subs":[]},
	               {"type":"type", "name":["alloc"], "subs":[]}
	               ]} )"_bj);
	BOOST_TEST(make_json(*bar2_type, tree) == 
	               R"( {"type":"type", "name":["list"], "subs":[
	               {"type":"type", "name":["foo"], "subs":[
	                 { "type":"type", "name":["str_t"], "subs":[] }]}
	               ]} )"_bj);

	auto ret_links = ret_type->required_links();
	BOOST_TEST_REQUIRE(ret_links.size()==1);
	BOOST_TEST_REQUIRE(ret_links[0].size()==1);
	BOOST_TEST(ret_links[0][0]=="int");
}
BOOST_AUTO_TEST_CASE(standard_types)
{
	using ix3::utils::details::ix3_node_base;
	using gu_tree = gen_utils::tree;
	using gu_ctx = gen_utils::compilation_context;
	to_generic_ast maker;
	auto ast = txt::parse(txt::file_content,
	                      "module mod1 v1.1:"
	                      "list<i8> foo(+list a);");
	gen_utils::tree tree = maker(ast.modules);
	auto mod = tree.children(*tree.children(tree.root()).at(0)).at(0);
	BOOST_TEST(tree.children(*mod).size()==1);

	auto std_i8 = gen_utils_mocks::make_node(10, std::nullopt, std::nullopt, "");
	auto i8_cpp = gen_utils_mocks::make_node(11, std::nullopt, std::nullopt, "i8");
	auto i8_js = gen_utils_mocks::make_node(12, std::nullopt, std::nullopt, "i8");
	auto list_cpp = gen_utils_mocks::make_node(12, std::nullopt, std::nullopt, "list");
	auto std_dsl = std::make_shared<gen_utils_mocks::dsl_manager>();
	MOCK_EXPECT(std_dsl->id).returns("std_types");
	MOCK_EXPECT(std_dsl->to_json).calls(
	            [&i8_cpp, &i8_js, &list_cpp](const gu_ctx& ctx, const gu_tree& con){
		BOOST_TEST(ctx.linked_to != nullptr);
		boost::json::value ret;
		if(ctx.linked_to == i8_cpp) ret = "std::int8_t"s;
		if(ctx.linked_to == i8_js) ret = "integer"s;
		if(ctx.linked_to == list_cpp) ret = "std::vector"s;
		return ret;
	});
	MOCK_EXPECT(i8_cpp->link_condition).returns("cpp");
	MOCK_EXPECT(i8_js->link_condition).returns("js");
	MOCK_EXPECT(list_cpp->link_condition).returns("cpp");
	MOCK_EXPECT(i8_cpp->imports_modification).returns(std::nullopt);
	MOCK_EXPECT(i8_js->imports_modification).returns(std::nullopt);
	MOCK_EXPECT(list_cpp->imports_modification).returns(std::nullopt);
	gen_utils::tree std_types(gen_utils_mocks::make_node(1), std_dsl);
	std_types.add(std_types.root(), std_i8);
	std_types.add(*std_i8, i8_cpp);
	std_types.add(*std_i8, i8_js);
	std_types.add(std_types.root(), list_cpp);
	gen_utils::imports_manager im;
	gen_utils::input file_data;
	file_data.add(std_types);
	file_data.add(tree);
	im("file", file_data).build();

	auto foo = tree.children(*mod).at(0);

	gu_ctx test_ctx;
	test_ctx.links = &im;
	fake_compiler fc{test_ctx.cfg};
	ix3::utils::details::compilation_context ctx(&tree, &fc, &test_ctx);
	auto json = static_cast<const ix3_node_base&>(*foo).make_json(ctx);

	BOOST_TEST(json == R"({
	               "orig_name":"foo","name":"foo",
	               "type":"function",
	               "params":[ {
	                 "orig_name":"a", "name":"a", "type":"function_parameter", "req":true,
	                 "param_t": { "type":"type",
	                   "cpp":"std::vector", "name":["list"], "subs":[] }
	               }],
	               "return":{
	                 "type":"type",
	                 "cpp":"std::vector",
	                 "name":["list"],
	                 "subs":[
	                   { "type":"type",
	                     "cpp":"std::int8_t",
	                     "js":"integer",
	                     "name":["i8"],
	                     "subs":[]
	                   }
	                 ]}
	               })"_bj);
}
BOOST_AUTO_TEST_CASE(enums)
{
	to_generic_ast maker;
	auto ast = txt::parse(txt::file_content,
	                      "module mod1 v1.1:"
	                      "enum bar_baz +auto_io { one, two => \"test\" }"sv);
	gen_utils::tree tree = maker(ast.modules);
	BOOST_TEST(tree.children(tree.root()).size()==1);
	auto mod = tree.children(*tree.children(tree.root()).at(0)).at(0);
	BOOST_TEST(tree.children(*mod).size()==1);

	auto e = tree.children(*mod).at(0);
	BOOST_TEST(e->name()=="bar_baz");
	BOOST_CHECK(!e->version().has_value());
	BOOST_TEST(e->required_links().size() == 0);

	BOOST_TEST(make_json(*e, tree) == boost::json::parse(
	               R"({
	                 "type":"enum","name":"bar_baz", "orig_name":"bar_baz",
	                 "auto_io":true, "as_flags":false,
	                 "members":[
	                   {"name":"one", "io":"one"},
	                   {"name":"two", "io":"test"}
	                 ]
	               })"));

	check_naming(*e, tree, {"BarBaz"s, "BarBaz"s, "bar_baz"s});
}
BOOST_AUTO_TEST_CASE(places)
{
	auto ast = txt::parse(txt::file_content,
	                      "module mod1 v1.1:"
	                      "enum z{o}"
	                      "interface bar_baz +ex { }"
			      ""sv);
	gen_utils::compilation_context ctx;
	auto tree = to_generic_ast()(ast.modules);
	BOOST_TEST(tree.to_json(ctx) == R"([{"name":"mod1", "orig_name":"mod1","vers":[
	                 {"type":"version","minor":1,"major":1,"name":"mod1_v1_1","content":[
	                 {"type":"enum","orig_name":"z","name":"z","auto_io":false,"as_flags":false,"members":[{"name":"o","io":"o"}]},
	                 {"type":"interface","orig_name":"bar_baz","name":"bar_baz","ex":true,"rinvert":false,"ctors":[],"funcs":[]},
	                 ]}
	               ]}])"_bj);
}
BOOST_AUTO_TEST_CASE(interface)
{
	auto ast = txt::parse(txt::file_content,
	                      "module mod1 v1.1:"
	                      "interface bar_baz +ex { "
	                        "constructor(+i8 a); "
	                        "i8 foo(+i9 b) const; } "
	                      "enum z{o}"sv);
	gen_utils::tree tree = to_generic_ast()(ast.modules);
	BOOST_TEST(tree.children(tree.root()).size()==1);
	BOOST_TEST(tree.children(*tree.children(tree.root()).at(0)).size()==1);
	auto mod = tree.children(*tree.children(tree.root()).at(0)).at(0);
	BOOST_TEST(tree.children(*mod).size()==2);

	BOOST_TEST(tree.children(*mod).at(1)->name() == "z");

	auto e = tree.children(*mod).at(0);
	BOOST_TEST(e->name()=="bar_baz");
	BOOST_CHECK(!e->version().has_value());
	BOOST_TEST(e->required_links().size() == 0);
	BOOST_TEST(e->link_condition() == "ix3"sv);

	BOOST_TEST(make_json(*e, tree) == R"({
	                 "type":"interface","name":"bar_baz", "orig_name":"bar_baz",
	                 "ex":true, "rinvert":false,
	                 "ctors":[ {"type":"ctor", "params":[{
	                    "orig_name":"a","name":"a",
	                    "param_t":{"type":"type", "name":["i8"], "subs":[]},
	                    "type":"function_parameter",
	                    "req":true }]} ],
	                 "funcs":[ {
	                   "type":"function","name":"foo", "orig_name":"foo",
	                   "return":{"type":"type", "name":["i8"], "subs":[]},
	                   "params":[{
	                    "orig_name":"b","name":"b",
	                    "param_t":{"type":"type", "name":["i9"], "subs":[]},
	                    "type":"function_parameter",
	                    "req":true }]
	                 } ]
	               })"_bj);
	check_naming(*e, tree, {"BarBaz"s, "BarBaz"s, "bar_baz"s});
}
BOOST_AUTO_TEST_CASE(meta)
{
	auto ast = txt::parse(txt::file_content,
	                      "module mod1 v1.1:\n"
			      "@v1.2\n @depricated v1.2 (\"test\")\n"
	                      "interface bar_baz +ex { "
	                        "constructor(+i8 a); "
	                        "i8 foo(+i9 b) const; }\n"
	                      "@depricated v1.3 (\"no\")\n"
	                      "#doc string\n"
	                      "enum z{o}"sv);
	gen_utils::tree tree = to_generic_ast()(ast.modules);
	BOOST_TEST(tree.children(*tree.children(tree.root()).at(0)).size()==1);
	auto mod = tree.children(*tree.children(tree.root()).at(0)).at(0);
	BOOST_TEST(tree.children(*mod).size()==2);

	BOOST_TEST(*mod->version() == ix3::utils::details::splash_version(1,1));

	auto z = tree.children(*mod).at(1);
	BOOST_TEST(z->name() == "z");

	auto bar_baz = tree.children(*mod).at(0);
	BOOST_TEST(bar_baz->name() == "bar_baz");
	BOOST_TEST(*bar_baz->version() == ix3::utils::details::splash_version(1,2));

	BOOST_TEST(make_json(*bar_baz, tree).as_object()["meta"] ==
			R"({"depricated":{"msg":"test","since":{"major":1,"minor":2}}})"_bj);
	BOOST_TEST(make_json(*z, tree).as_object()["meta"] == R"({"doc":"doc string"})"_bj);
}
BOOST_AUTO_TEST_CASE(pop_parent)
{
	to_generic_ast maker;
	auto ast = txt::parse(txt::file_content,
	                      "module mod1 v1.1:"
	                      "record r {-int f1;}"
	                      "i8 foo();"sv);
	gen_utils::tree tree = maker(ast.modules);
	BOOST_TEST(tree.children(tree.root()).size()==1);
	auto mod = tree.children(*tree.children(tree.root()).at(0)).at(0);
	BOOST_TEST(tree.children(*mod).size()==2);

}
BOOST_AUTO_TEST_CASE(lelf_links)
{
	auto ast_f1 = txt::parse(txt::file_content,
	                      "module mod1 v1.1:"
	                      "interface inter {  }"sv);
	auto ast_f2 = txt::parse(txt::file_content,
	                      "module mod2 v1.1:"
	                      "mod1.inter foo();"sv);
	gen_utils::tree tree_f1 = to_generic_ast()(ast_f1.modules);
	gen_utils::tree tree_f2 = to_generic_ast()(ast_f2.modules);

	gen_utils::input f1, f2;
	f1.add(tree_f1).conf().naming.at(0) = gen_utils::name_conversion::title_case;
	f2.add(tree_f2);

	BOOST_TEST(tree_f2.children(tree_f2.root()).size()==1);
	auto mod = tree_f2.children(*tree_f2.children(tree_f2.root()).at(0)).at(0);
	BOOST_TEST(tree_f2.children(*mod).size()==1);
	auto foo = tree_f2.children(*mod).at(0);

	gen_utils::imports_manager imng;
	imng.add("f1", f1).add("f2", f2).build();
	BOOST_TEST(imng.required_for(f2).size()==1);
	BOOST_TEST(imng.required_for(f2).at(0).file.name=="f1");

	gen_utils::compilation_context ctx;
	ctx.links = &imng;
	BOOST_TEST(make_json(*foo, tree_f2, ctx).as_object()["return"] == R"({
	                    "type":"type","name":["mod1","inter"],"subs":[],
	                    "ix3":{"type":"interface",
	                        "orig_name":"inter", "name":"Inter",
	                        "ex":false,"rinvert":false,"ctors":[],"funcs":[]}
	                    })"_bj);
}
BOOST_AUTO_TEST_SUITE_END() // gain_to_generic_ast

BOOST_AUTO_TEST_SUITE(ix3_manager_suite)
BOOST_AUTO_TEST_CASE(getters)
{
	ix3::utils::ix3_manager mng;
	BOOST_TEST(mng.id() == "ix3"sv);
}
BOOST_AUTO_TEST_CASE(to_json)
{
	struct fake_ix3_node : ix3::utils::details::ix3_node_base {
		boost::json::object js;
		fake_ix3_node(boost::json::object j) : js(std::move(j)) { }
		std::string_view name() const override { return "fake_ix3_node"sv; }
		std::optional<std::uint64_t> version() const override { return std::nullopt; }
		boost::json::value make_json(const ix3::utils::details::compilation_context&) const override
		{
			return js;
		}
	};

	boost::json::object js;

	auto mng = std::make_shared<ix3::utils::ix3_manager>();
	auto root = std::make_shared<ix3::utils::details::ix3_root_node>();
	gen_utils::tree t1(root, mng);
	auto child1 = std::make_shared<fake_ix3_node>(js);
	auto child11 = std::make_shared<fake_ix3_node>((js["test"]="ok", js));
	t1.add(*root, child1);
	t1.add(*child1, child11);
	gen_utils::compilation_context ctx;
	gen_utils::imports_manager imports;
	ctx.linked_to = child11;
	ctx.links = &imports;
	BOOST_TEST(t1.to_json(ctx) == child11->js);

}
BOOST_AUTO_TEST_SUITE_END() // ix3_manager_suite

BOOST_AUTO_TEST_SUITE(cpp_compiler)
BOOST_AUTO_TEST_SUITE_END() // cpp_compiler

BOOST_AUTO_TEST_SUITE_END() // utils
BOOST_AUTO_TEST_SUITE_END() // ix3
