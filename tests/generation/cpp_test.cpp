/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE modegen_parser

#include <boost/test/unit_test.hpp>
#include <turtle/mock.hpp>

#include "errors.h"
#include "generation/cpp.hpp"
#include "generation/output_info.hpp"
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

MOCK_BASE_CLASS( part_desc_mock, modegen::generation::part_descriptor )
{
	MOCK_METHOD( part_name, 0 )
	MOCK_METHOD( file_name, 0 )
	MOCK_METHOD( opts, 0 )
	MOCK_METHOD( need_output, 0 )
	MOCK_METHOD( next, 0 )
	MOCK_METHOD( reset, 0 )
	MOCK_METHOD( idl_input, 0 )
	MOCK_METHOD( data_input, 0 )
	static std::vector<mi::module> mods;
};

std::vector<mi::module> part_desc_mock::mods;

struct ab_part_desc : modegen::generation::part_descriptor {
	mo::view opts_;
	mutable int cur_iter = 0;
	ab_part_desc(mo::view o) : opts_(std::move(o)) {}

	std::string part_name() const override {return std::string(opts_.part()); }
	const mo::view& opts() const override {return opts_;}
	bool need_output() const override {return cur_iter < 2;}
	bool next() override {return true;}
	std::string file_name() const override
	{
		throw 1;
		return ""s;
	}

	std::vector<mi::module> idl_input() const override { return {}; }
	boost::property_tree::ptree data_input() const override
	{
		boost::property_tree::ptree ret;
		return ret;
	}
};

BOOST_AUTO_TEST_CASE( no_data_is_error )
{
	auto opts_tree = std::make_shared<mo::container>();
	opts_tree->raw().put("gen.def.input", "fake_tmpl");
	mg::cpp_generator gen;
	mo::view opts(opts_tree, "def"sv);

	auto pd = std::make_unique<part_desc_mock>();
	MOCK_EXPECT( pd->idl_input ).exactly(1).returns( part_desc_mock::mods );
	MOCK_EXPECT( pd->reset ).exactly(1).returns( pd.get() );
	mg::output_info oi;
	oi.add_part(std::move(pd)).next();
	BOOST_CHECK_THROW(gen.jsoned_data(oi), modegen::errors::gen_error);
}

BOOST_AUTO_TEST_CASE( extra_namespaces )
{
	auto opts_tree = std::make_shared<mo::container>();
	opts_tree->raw().put("gen.def.input", "fake_tmpl");
	opts_tree->raw().add("filegen.cpp.namespaces", "ns1");
	opts_tree->raw().add("filegen.cpp.namespaces", "ns2");
	mg::cpp_generator gen;
	mo::view opts(opts_tree, "def"sv);

	auto pd = std::make_unique<part_desc_mock>();
	MOCK_EXPECT( pd->idl_input ).once().returns( mi::parse("module mod v1.0: int foo();"sv).mods );
	MOCK_EXPECT( pd->data_input ).once().returns( pt::ptree{} );
	MOCK_EXPECT( pd->opts ).once().returns( opts );
	MOCK_EXPECT( pd->reset ).exactly(1).returns( pd.get() );

	mg::output_info oi;
	oi.add_part(std::move(pd)).next();

	nlohmann::json data = gen.jsoned_data( oi );
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
	mg::cpp_generator gen;
	mg::options::view opts(opts_tree, "def"sv);

	pt::ptree extra_data;
	extra_data.put("some", "data");

	auto pd = std::make_unique<part_desc_mock>();
	MOCK_EXPECT( pd->idl_input ).once().returns( mi::parse("module mod v1.0: int foo();"sv).mods );
	MOCK_EXPECT( pd->data_input ).once().returns( extra_data );
	MOCK_EXPECT( pd->opts ).once().returns( opts );
	MOCK_EXPECT( pd->reset ).exactly(1).returns( pd.get() );

	mg::output_info oi;
	oi.add_part(std::move(pd)).next();
	nlohmann::json data = gen.jsoned_data( oi );
	BOOST_CHECK_EQUAL(data["extra_data"s]["some"s], "data"s);

	std::stringstream out;
	BOOST_CHECK_NO_THROW(out << data);
}

BOOST_AUTO_TEST_SUITE(prefix_suffix)
BOOST_AUTO_TEST_CASE( ctor_prefix_ptr_siffux )
{
	auto opts_tree = std::make_shared<mo::container>();
	opts_tree->raw().put("gen.def.input", "fake_tmpl");
	opts_tree->raw().add("filegen.cpp.ctor_prefix", "ctor_");
	opts_tree->raw().add("filegen.cpp.ptr_suffix", "_ptR");
	mg::cpp_generator gen;
	mg::options::view opts(opts_tree, "def"sv);

	auto pd = std::make_unique<part_desc_mock>();
	MOCK_EXPECT( pd->idl_input ).once().returns( mi::parse("module mod v1.0: int foo();"sv).mods );
	MOCK_EXPECT( pd->data_input ).once().returns( pt::ptree{} );
	MOCK_EXPECT( pd->opts ).once().returns( opts );
	MOCK_EXPECT( pd->reset ).exactly(1).returns( pd.get() );

	mg::output_info oi;
	oi.add_part( std::move(pd) ).next();
	nlohmann::json data = gen.jsoned_data( oi );
	BOOST_CHECK_EQUAL( data["ctor_prefix"], "ctor_" );
	BOOST_CHECK_EQUAL( data["ptr_suffix"], "_ptR" );
}
BOOST_AUTO_TEST_CASE( defaults )
{
	auto opts_tree = std::make_shared<mo::container>();
	opts_tree->raw().put("gen.def.input", "fake_tmpl");
	mg::cpp_generator gen;
	mg::options::view opts(opts_tree, "def"sv);

	auto pd = std::make_unique<part_desc_mock>();
	MOCK_EXPECT( pd->idl_input ).exactly(5).returns( mi::parse("module mod v1.0: int foo();"sv).mods );
	MOCK_EXPECT( pd->data_input ).exactly(5).returns( pt::ptree{} );
	MOCK_EXPECT( pd->opts ).exactly(5).returns( opts );
	MOCK_EXPECT( pd->reset ).exactly(1).returns( pd.get() );

	mg::output_info oi;
	oi.add_part( std::move(pd) ).next();

	nlohmann::json data = gen.jsoned_data(oi);
	BOOST_CHECK_EQUAL( data["ctor_prefix"], "create_" );
	BOOST_CHECK_EQUAL( data["ptr_suffix"], "_ptr" );

	opts_tree->raw().put("gen.def.naming", "title");
	data = gen.jsoned_data(oi);
	BOOST_CHECK_EQUAL( data["ctor_prefix"], "Create" );
	BOOST_CHECK_EQUAL( data["ptr_suffix"], "Ptr" );

	opts_tree->raw().put("gen.def.naming", "camel");
	data = gen.jsoned_data(oi);
	BOOST_CHECK_EQUAL( data["ctor_prefix"], "create" );
	BOOST_CHECK_EQUAL( data["ptr_suffix"], "Ptr" );

	opts_tree->raw().put("gen.def.naming", "underscore");
	data = gen.jsoned_data(oi);
	BOOST_CHECK_EQUAL( data["ctor_prefix"], "create_" );
	BOOST_CHECK_EQUAL( data["ptr_suffix"], "_ptr" );

	opts_tree->raw().put("gen.def.naming", "asis");
	data = gen.jsoned_data(oi);
	BOOST_CHECK_EQUAL( data["ctor_prefix"], "create_" );
	BOOST_CHECK_EQUAL( data["ptr_suffix"], "_ptr" );
}
BOOST_AUTO_TEST_SUITE_END() // prefix_suffix

BOOST_AUTO_TEST_SUITE(includes)
BOOST_AUTO_TEST_CASE( without_includes )
{
	auto opts_tree = std::make_shared<mo::container>();
	opts_tree->raw().put("gen.def.input", "fake_tmpl");
	mg::cpp_generator gen;
	mg::options::view opts(opts_tree, "def"sv);

	auto pd = std::make_unique<part_desc_mock>();
	MOCK_EXPECT( pd->idl_input ).exactly(1).returns( mi::parse("module mod v1.0: int foo();"sv).mods );
	MOCK_EXPECT( pd->data_input ).exactly(1).returns( pt::ptree{} );
	MOCK_EXPECT( pd->opts ).exactly(1).returns( opts );
	MOCK_EXPECT( pd->reset ).exactly(1).returns( pd.get() );

	mg::output_info oi;
	oi.add_part( std::move(pd) ).next();

	nlohmann::json data = gen.jsoned_data(oi);
	BOOST_REQUIRE_EQUAL( data["mods"].size(), 1 );
	BOOST_REQUIRE_EQUAL( data["mods"][0]["content"].size(), 1 );
	BOOST_CHECK_EQUAL( data["mods"][0]["content"][0]["type"], "function" );

	BOOST_REQUIRE( !data.contains("incs") );
}
BOOST_AUTO_TEST_CASE(with_lnag_includes)
{
	auto opts_tree = std::make_shared<mo::container>();
	opts_tree->raw().put("gen.def.input", "fake_tmpl");
	mg::cpp_generator gen;
	mg::options::view opts(opts_tree, "def"sv);

	auto pd = std::make_unique<part_desc_mock>();
	MOCK_EXPECT( pd->idl_input ).exactly(1).returns( mi::parse("module mod v1.0: string foo(date d, list<i8> l);"sv).mods );
	MOCK_EXPECT( pd->data_input ).exactly(1).returns( pt::ptree{} );
	MOCK_EXPECT( pd->opts ).exactly(1).returns( opts );
	MOCK_EXPECT( pd->reset ).exactly(1).returns( pd.get() );

	mg::output_info oi;
	oi.add_part( std::move(pd) ).next();

	nlohmann::json data = gen.jsoned_data(oi);
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

	auto pd = std::make_unique<part_desc_mock>();
	MOCK_EXPECT( pd->idl_input ).exactly(1).returns( mi::parse("module mod v1.0: string foo(date d, list<i8> l);"sv).mods );
	MOCK_EXPECT( pd->data_input ).exactly(1).returns( pt::ptree{} );
	MOCK_EXPECT( pd->opts ).exactly(1).returns( opts );
	MOCK_EXPECT( pd->reset ).exactly(1).returns( pd.get() );

	mg::output_info oi;
	oi.add_part(std::move(pd)).next();
	nlohmann::json data = gen.jsoned_data(oi);

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
//BOOST_AUTO_TEST_CASE( no_part_opts )
//{
	//auto opts_tree = std::make_shared<mo::container>();
	//mg::cpp_generator gen;
	//mg::options::view opts(opts_tree, "def"sv);

	//auto pd = std::make_unique<part_desc_mock>();
	//MOCK_EXPECT( pd->idl_input ).exactly(1).returns( mi::parse("module mod v1.0: string foo(date d, list<i8> l);"sv).mods );
	//MOCK_EXPECT( pd->opts ).exactly(1).returns( opts );
	//MOCK_EXPECT( pd->data_input ).exactly(1).returns( pt::ptree{} );
	//MOCK_EXPECT( pd->reset ).exactly(1).returns( pd.get() );

	//mg::output_info oi;
	//oi.add_part(std::move(pd)).next();
	//BOOST_CHECK_THROW( gen.jsoned_data(oi), modegen::errors::error );
//}
BOOST_AUTO_TEST_SUITE_END() // error_data
