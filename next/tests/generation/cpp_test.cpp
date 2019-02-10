#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE modegen_parser

#include <boost/test/unit_test.hpp>
#include <turtle/mock.hpp>

#include "errors.h"
#include "generation/cpp.hpp"
#include "parser/interface/loader.hpp"

// tests cpp generation (don't test correctly working of filters)

namespace mi = modegen::parser::interface;
namespace mg = modegen::generation;
namespace pt = boost::property_tree;

using namespace std::literals;

struct fake_mi_loader : public mi::loader {
	std::vector<mi::module> data;
	
	void load(std::istream& input, std::string fn) override {}
	void load(std::filesystem::path file) override {}
	void finish_loads() override {}

	std::vector<mi::module> result() const override
	{
		return data;
	}
};

BOOST_AUTO_TEST_CASE( no_data_no_work )
{
	pt::ptree opts_tree;
	mg::cpp_generator gen;
	mg::options_view opts(opts_tree, "def"sv);

	cppjson::value data = gen.jsoned_data(std::make_shared<fake_mi_loader>(), opts);
	BOOST_CHECK(data.is_undefined());
}

BOOST_AUTO_TEST_CASE( without_includes )
{
	pt::ptree opts_tree;
	mg::cpp_generator gen;
	mg::options_view opts(opts_tree, "def"sv);

	auto ldr = std::make_shared<fake_mi_loader>();
	ldr->data = mi::parse("module mod v1.0: int foo();"sv).mods;

	cppjson::value data = gen.jsoned_data(ldr, opts);
	BOOST_REQUIRE(!data.is_undefined());
	BOOST_REQUIRE_EQUAL( data["mods"].array().size(), 1 );
	BOOST_REQUIRE_EQUAL( data["mods"][0]["content"].array().size(), 1 );
	BOOST_CHECK_EQUAL( data["mods"][0]["content"][0]["type"], "function" );

	BOOST_REQUIRE( data["incs"].is_undefined() );
}

BOOST_AUTO_TEST_CASE(with_sys_includes)
{
	pt::ptree opts_tree;
	mg::cpp_generator gen;
	mg::options_view opts(opts_tree, "def"sv);

	auto ldr = std::make_shared<fake_mi_loader>();
	ldr->data = mi::parse("module mod v1.0: string foo(date d, list<i8> l);"sv).mods;

	cppjson::value data = gen.jsoned_data(ldr, opts);
	BOOST_REQUIRE( !data["incs"].is_undefined() );
	BOOST_REQUIRE_EQUAL( data["incs"].array().size(), 4 );
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

BOOST_AUTO_TEST_CASE(with_local_includes)
{
	BOOST_FAIL("no test");
}

BOOST_AUTO_TEST_SUITE( error_data )
BOOST_AUTO_TEST_CASE(wrong_loader)
{
	struct fake_wrong_loader : public modegen::parser::loader {
		void load(std::istream& input, std::string fn) override {}
		void load(std::filesystem::path file) override {}
		void finish_loads() override {}
	};

	pt::ptree opts_tree;
	mg::cpp_generator gen;
	mg::options_view opts(opts_tree, "def"sv);

	BOOST_CHECK_THROW( gen.jsoned_data(nullptr, opts), modegen::errors::gen_error );
	BOOST_CHECK_THROW( gen.jsoned_data(std::make_shared<fake_wrong_loader>(), opts), modegen::errors::gen_error );
}
BOOST_AUTO_TEST_SUITE_END() // error_data
