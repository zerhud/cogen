/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE cpp file generator

#include <boost/test/unit_test.hpp>
#include <turtle/mock.hpp>

#include "errors.h"
#include "generation/cpp.hpp"
#include "parser/interface/loader.hpp"
#include "parser/data_tree/loader.hpp"

// tests cpp generation (don't test correctly working of filters)

namespace mi = modegen::parser::interface;
namespace mg = modegen::generation;
namespace mo = modegen::generation::options;
namespace pt = boost::property_tree;

using namespace std::literals;

struct fake_mi_loader : public mi::loader {
	std::vector<mi::module> data;
	
	void load(std::istream& input, std::string fn) override {}
	void load(FS::path file) override {}
	void finish_loads() override {}

	std::vector<mi::module> result() const override
	{
		return data;
	}
};

struct fake_data_tree_loader : public modegen::parser::data_tree::loader {
	pt::ptree data;

	void load(std::istream& input, std::string fn) override {}
	void load(FS::path file) override {}
	void finish_loads() override {}

	pt::ptree boost_ptree() const override
	{
		return data;
	}
};

BOOST_AUTO_TEST_CASE( no_data_is_error )
{
	auto opts_tree = std::make_shared<mo::container>();
	opts_tree->raw().put("gen.def.input", "fake_tmpl");
	mg::cpp_generator gen;
	mo::view opts(opts_tree, "def"sv);

	BOOST_CHECK_THROW(gen.jsoned_data({std::make_shared<fake_mi_loader>()}, opts), modegen::errors::gen_error);
}

BOOST_AUTO_TEST_CASE( extra_namespaces )
{
	auto opts_tree = std::make_shared<mo::container>();
	opts_tree->raw().put("gen.def.input", "fake_tmpl");
	opts_tree->raw().put("gen.def.output", "o");
	opts_tree->raw().add("filegen.cpp.namespaces", "ns1");
	opts_tree->raw().add("filegen.cpp.namespaces", "ns2");
	mg::cpp_generator gen;
	mo::view opts(opts_tree, "def"sv);

	auto ldr = std::make_shared<fake_mi_loader>();
	ldr->data = mi::parse("module mod v1.0: int foo();"sv).mods;

	auto _data = gen.jsoned_data({ldr}, opts);
	BOOST_REQUIRE_EQUAL(_data.size(), 1);
	BOOST_CHECK_EQUAL(_data[0].out_file, "o");

	nlohmann::json& data = _data[0].data;
	BOOST_REQUIRE_EQUAL( data["namespaces"].size(), 2 );
	BOOST_CHECK_EQUAL( data["namespaces"][0], "ns1" );
	BOOST_CHECK_EQUAL( data["namespaces"][1], "ns2" );

	std::stringstream out;
	BOOST_CHECK_NO_THROW(out << data);
}

BOOST_AUTO_TEST_CASE(extra_data)
{
	auto opts_tree = std::make_shared<mo::container>();
	opts_tree->raw().put("gen.def.input", "fake_tmpl");
	opts_tree->raw().put("gen.def.output", "o");
	mg::cpp_generator gen;
	mg::options::view opts(opts_tree, "def"sv);

	auto ldr = std::make_shared<fake_mi_loader>();
	ldr->data = mi::parse("module mod v1.0: int foo();"sv).mods;

	auto dldr = std::make_shared<fake_data_tree_loader>();
	dldr->data.put("some", "data");

	auto _data = gen.jsoned_data({ldr,dldr}, opts);
	BOOST_REQUIRE_EQUAL(_data.size(), 1);
	BOOST_CHECK_EQUAL(_data[0].out_file, "o");

	nlohmann::json& data = _data[0].data;
	BOOST_CHECK_EQUAL(data["extra_data"s]["some"s], "data"s);

	auto _data2 = gen.jsoned_data({ldr,nullptr,dldr}, opts);
	BOOST_REQUIRE_EQUAL(_data2.size(), 1);

	nlohmann::json& data2 = _data2[0].data;
	BOOST_CHECK_EQUAL(data2["extra_data"s]["some"s], "data"s);

	std::stringstream out;
	BOOST_CHECK_NO_THROW(out << data2);
}

BOOST_AUTO_TEST_SUITE(prefix_suffix)
BOOST_AUTO_TEST_CASE( ctor_prefix_ptr_siffux )
{
	auto opts_tree = std::make_shared<mo::container>();
	opts_tree->raw().put("gen.def.input", "fake_tmpl");
	opts_tree->raw().put("gen.def.output", "o");
	opts_tree->raw().add("filegen.cpp.ctor_prefix", "ctor_");
	opts_tree->raw().add("filegen.cpp.ptr_suffix", "_ptR");
	mg::cpp_generator gen;
	mg::options::view opts(opts_tree, "def"sv);

	auto ldr = std::make_shared<fake_mi_loader>();
	ldr->data = mi::parse("module mod v1.0: int foo();"sv).mods;

	auto _data = gen.jsoned_data({ldr}, opts);
	BOOST_REQUIRE_EQUAL(_data.size(), 1);
	BOOST_CHECK_EQUAL(_data[0].out_file, "o");

	nlohmann::json& data = _data[0].data;
	BOOST_CHECK_EQUAL( data["ctor_prefix"], "ctor_" );
	BOOST_CHECK_EQUAL( data["ptr_suffix"], "_ptR" );
}
BOOST_AUTO_TEST_CASE( defaults )
{
	auto opts_tree = std::make_shared<mo::container>();
	opts_tree->raw().put("gen.def.input", "fake_tmpl");
	opts_tree->raw().put("gen.def.output", "out");
	mg::cpp_generator gen;
	mg::options::view opts(opts_tree, "def"sv);

	auto ldr = std::make_shared<fake_mi_loader>();
	ldr->data = mi::parse("module mod v1.0: int foo();"sv).mods;

	auto _data = gen.jsoned_data({ldr}, opts);
	BOOST_REQUIRE_EQUAL(_data.size(), 1);
	BOOST_CHECK_EQUAL(_data[0].out_file, "out");

	nlohmann::json data = std::move(_data[0].data);
	BOOST_CHECK_EQUAL( data["ctor_prefix"], "create_" );
	BOOST_CHECK_EQUAL( data["ptr_suffix"], "_ptr" );

	opts_tree->raw().put("gen.def.naming", "title");
	data = gen.jsoned_data({ldr}, opts)[0].data;
	BOOST_CHECK_EQUAL( data["ctor_prefix"], "Create" );
	BOOST_CHECK_EQUAL( data["ptr_suffix"], "Ptr" );

	opts_tree->raw().put("gen.def.naming", "camel");
	data = gen.jsoned_data({ldr}, opts)[0].data;
	BOOST_REQUIRE_EQUAL(_data.size(), 1);
	BOOST_CHECK_EQUAL( data["ctor_prefix"], "create" );
	BOOST_CHECK_EQUAL( data["ptr_suffix"], "Ptr" );

	opts_tree->raw().put("gen.def.naming", "underscore");
	data = gen.jsoned_data({ldr}, opts)[0].data;
	BOOST_REQUIRE_EQUAL(_data.size(), 1);
	BOOST_CHECK_EQUAL( data["ctor_prefix"], "create_" );
	BOOST_CHECK_EQUAL( data["ptr_suffix"], "_ptr" );

	opts_tree->raw().put("gen.def.naming", "asis");
	data = gen.jsoned_data({ldr}, opts)[0].data;
	BOOST_REQUIRE_EQUAL(_data.size(), 1);
	BOOST_CHECK_EQUAL( data["ctor_prefix"], "create_" );
	BOOST_CHECK_EQUAL( data["ptr_suffix"], "_ptr" );
}
BOOST_AUTO_TEST_SUITE_END() // prefix_suffix

BOOST_AUTO_TEST_SUITE(includes)
BOOST_AUTO_TEST_CASE( without_includes )
{
	auto opts_tree = std::make_shared<mo::container>();
	opts_tree->raw().put("gen.def.input", "fake_tmpl");
	opts_tree->raw().put("gen.def.output", "o");
	mg::cpp_generator gen;
	mg::options::view opts(opts_tree, "def"sv);

	auto ldr = std::make_shared<fake_mi_loader>();
	ldr->data = mi::parse("module mod v1.0: int foo();"sv).mods;

	auto _data = gen.jsoned_data({ldr}, opts);
	BOOST_REQUIRE_EQUAL(_data.size(), 1);
	BOOST_CHECK_EQUAL(_data[0].out_file, "o");

	nlohmann::json& data = _data[0].data;
	BOOST_REQUIRE_EQUAL( data["mods"].size(), 1 );
	BOOST_REQUIRE_EQUAL( data["mods"][0]["content"].size(), 1 );
	BOOST_CHECK_EQUAL( data["mods"][0]["content"][0]["type"], "function" );

	BOOST_REQUIRE( !data.contains("incs") );
}
BOOST_AUTO_TEST_CASE(with_lnag_includes)
{
	auto opts_tree = std::make_shared<mo::container>();
	opts_tree->raw().put("gen.def.input", "fake_tmpl");
	opts_tree->raw().put("gen.def.output", "o");
	mg::cpp_generator gen;
	mg::options::view opts(opts_tree, "def"sv);

	auto ldr = std::make_shared<fake_mi_loader>();
	ldr->data = mi::parse("module mod v1.0: string foo(date d, list<i8> l);"sv).mods;

	auto _data = gen.jsoned_data({ldr}, opts);
	BOOST_REQUIRE_EQUAL(_data.size(), 1);
	BOOST_CHECK_EQUAL(_data[0].out_file, "o");

	nlohmann::json& data = _data[0].data;
	BOOST_REQUIRE( data.contains("incs") );
	BOOST_REQUIRE_EQUAL( data["incs"].size(), 4 );
	// order sorted by alphabet
	BOOST_CHECK_EQUAL( data["incs"][0]["n"], "chrono" );
	BOOST_CHECK_EQUAL( data["incs"][1]["n"], "cstdint" );
	BOOST_CHECK_EQUAL( data["incs"][2]["n"], "string" );
	BOOST_CHECK_EQUAL( data["incs"][3]["n"], "vector" );
	BOOST_CHECK_EQUAL( data["incs"][0]["sys"], true );
	BOOST_CHECK_EQUAL( data["incs"][1]["sys"], true );
	BOOST_CHECK_EQUAL( data["incs"][2]["sys"], true );
	BOOST_CHECK_EQUAL( data["incs"][3]["sys"], true );
}
BOOST_AUTO_TEST_CASE(with_setts_includes)
{
	auto opts_tree = std::make_shared<mo::container>();
	opts_tree->raw().add("gen.hpp.output", "output");
	opts_tree->raw().add("gen.hpp.inc_part", "def");
	opts_tree->raw().add("gen.hpp.inc_part", "test");
	opts_tree->raw().add("gen.hpp.inc_local", "local.hpp");
	opts_tree->raw().add("gen.hpp.inc_local", "local2.hpp");
	opts_tree->raw().add("gen.hpp.inc_sys", "sys.hpp");
	opts_tree->raw().add("gen.hpp.inc_sys", "sys2.hpp");
	opts_tree->raw().put("gen.def.output", "definitions.hpp");
	opts_tree->raw().put("gen.test.output", "t/test.hpp");

	mg::cpp_generator gen;
	mg::options::view opts(opts_tree, "hpp"sv);

	auto ldr = std::make_shared<fake_mi_loader>();
	ldr->data = mi::parse("module mod v1.0: string foo(date d, list<i8> l);"sv).mods;
	nlohmann::json data = gen.jsoned_data({ldr}, opts)[0].data;

	BOOST_REQUIRE( data.contains("incs") );
	BOOST_REQUIRE_EQUAL( data["incs"].size(), 10 );
	BOOST_CHECK_EQUAL( data["incs"][0]["sys"], true );
	BOOST_CHECK_EQUAL( data["incs"][1]["sys"], true );
	BOOST_CHECK_EQUAL( data["incs"][2]["sys"], true );
	BOOST_CHECK_EQUAL( data["incs"][3]["sys"], true );
	BOOST_CHECK_EQUAL( data["incs"][0]["n"], "chrono" );
	BOOST_CHECK_EQUAL( data["incs"][1]["n"], "cstdint" );
	BOOST_CHECK_EQUAL( data["incs"][2]["n"], "string" );
	BOOST_CHECK_EQUAL( data["incs"][3]["n"], "vector" );

	BOOST_CHECK_EQUAL( data["incs"][4]["n"], "sys.hpp" );
	BOOST_CHECK_EQUAL( data["incs"][4]["sys"], true );
	BOOST_CHECK_EQUAL( data["incs"][5]["n"], "sys2.hpp" );
	BOOST_CHECK_EQUAL( data["incs"][5]["sys"], true );

	BOOST_CHECK_EQUAL( data["incs"][6]["n"], "definitions.hpp" );
	BOOST_CHECK_EQUAL( data["incs"][6]["sys"], false );

	BOOST_CHECK_EQUAL( data["incs"][7]["n"], "t/test.hpp" );
	BOOST_CHECK_EQUAL( data["incs"][7]["sys"], false );

	BOOST_CHECK_EQUAL( data["incs"][8]["n"], "local.hpp" );
	BOOST_CHECK_EQUAL( data["incs"][8]["sys"], false );
	BOOST_CHECK_EQUAL( data["incs"][9]["n"], "local2.hpp" );
	BOOST_CHECK_EQUAL( data["incs"][9]["sys"], false );
}
BOOST_AUTO_TEST_SUITE_END() // includes

BOOST_AUTO_TEST_SUITE( error_data )
BOOST_AUTO_TEST_CASE(wrong_loader)
{
	struct fake_wrong_loader : public modegen::parser::loader {
		void load(std::istream& input, std::string fn) override {}
		void load(FS::path file) override {}
		void finish_loads() override {}
	};

	auto opts_tree = std::make_shared<mo::container>();
	mg::cpp_generator gen;
	mg::options::view opts(opts_tree, "def"sv);

	BOOST_CHECK_THROW( gen.jsoned_data({nullptr}, opts), modegen::errors::gen_error );
	BOOST_CHECK_THROW( gen.jsoned_data({std::make_shared<fake_wrong_loader>()}, opts), modegen::errors::gen_error );
}
BOOST_AUTO_TEST_CASE(no_part_opts)
{
	auto opts_tree = std::make_shared<mo::container>();
	mg::cpp_generator gen;
	mg::options::view opts(opts_tree, "def"sv);

	BOOST_CHECK_THROW( gen.jsoned_data({std::make_shared<fake_mi_loader>()}, opts), modegen::errors::error );
}
BOOST_AUTO_TEST_CASE(no_output)
{
	auto opts_tree = std::make_shared<mo::container>();
	opts_tree->raw().put("gen.def.filegen", "cpp") ;
	opts_tree->raw().put("gen.def.input", "def.hpp.jinja") ;
	opts_tree->raw().put("gen.def.test", "test_data") ;

	mg::cpp_generator gen;
	mg::options::view opts(opts_tree, "def"sv);

	auto ldr = std::make_shared<fake_mi_loader>();
	ldr->data = mi::parse("module mod v1.0: string foo(date d, list<i8> l);"sv).mods;
	BOOST_CHECK_THROW( gen.jsoned_data({ldr}, opts), modegen::errors::error );
}
BOOST_AUTO_TEST_CASE(no_data)
{
	auto opts_tree = std::make_shared<mo::container>();

	mg::cpp_generator gen;
	mg::options::view opts(opts_tree, "def"sv);

	auto ldr = std::make_shared<fake_mi_loader>();
	BOOST_CHECK_THROW( gen.jsoned_data({ldr}, opts), modegen::errors::no_data );
}
BOOST_AUTO_TEST_SUITE_END() // error_data
