#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE modegen_parser

#include <boost/test/unit_test.hpp>
#include <turtle/mock.hpp>

#include "generation/common.hpp"
#include "generation/provider.hpp"
#include "generation/file_data.hpp"
#include "errors.h"

using namespace std::literals;
namespace mg = modegen::generation;
namespace mp = modegen::parser;
namespace fs = FS;

MOCK_BASE_CLASS( provider_mock, modegen::generation::provider )
{
	MOCK_METHOD( json_jinja, 3 )
	MOCK_METHOD( parser, 1 )
	MOCK_METHOD( generator, 1 )
};

class fake_target : public modegen::parser::loader {
	void load(std::istream& input, std::string fn) override {}
	void load(FS::path file) override {}
	void finish_loads() override {}
};

struct fake_data_gen : modegen::generation::file_data {
	bool gen_data = true;
	cppjson::value jsoned_data(mp::loader_ptr data_loader, mg::options_view opts) const override
	{
		cppjson::value ret;
		if(gen_data) {
			ret["name"] = std::string(opts.part_name());
			ret["test"] = opts.part_data().get<std::string>("test");
		}
		return ret;
	}
};

BOOST_AUTO_TEST_CASE(common_generation)
{
	// tests normal using
	//   1. create provider
	//     1. create all needed loaders
	//     2. set all needed generators (jinja only for now)
	//   2. set options in cg
	//   3. cg checks all loaders are provided (don't start generation before trust we have all info)
	//   4. for each resulting file
	//     1. cg calls file_data with it's loader for get data
	//     2. fd applies settings (filters, modificators etc)
	//     3. fd converts result to json
	//     4. cg generates resulting file for each item

	bool cout = true;
	auto result_data_checker = [&cout](const cppjson::value& data, const fs::path& tmpl, const std::optional<fs::path>& out) {
			if(cout) BOOST_CHECK( !out );
			else {
				BOOST_REQUIRE( out );
				BOOST_CHECK_EQUAL( *out, fs::path("some_dir") / "def.hpp" );
			}

			BOOST_CHECK_EQUAL( tmpl, fs::path(u8"some/path") / "definitions.hpp.jinja" );
			BOOST_CHECK_EQUAL( data["name"], "def"s );
			BOOST_CHECK_EQUAL( data["test"], "test_data"s );
			return true;
		};

	auto provider = std::make_shared<provider_mock>();
	MOCK_EXPECT( provider->json_jinja ).exactly(2).with( result_data_checker ) ;
	MOCK_EXPECT( provider->parser ).exactly(2).with( "interface"sv ).returns( std::make_shared<fake_target>() );
	MOCK_EXPECT( provider->generator ).exactly(2).with( "cpp"sv ).returns( std::make_shared<fake_data_gen>() );

	mg::generator gen(provider, u8"some/path");
	auto& opts = gen.options();
	opts.put("gen.def.target", "cpp") ;
	opts.put("gen.def.parser", "interface") ;
	opts.put("gen.def.output", "def.hpp") ;
	opts.put("gen.def.input", "definitions.hpp.jinja") ;
	opts.put("gen.def.test", "test_data") ;

	// first calls
	gen.generate_stdout("def");

	// second calls
	cout = false;
	gen.generate("some_dir");
}

BOOST_AUTO_TEST_SUITE(options_view)
BOOST_AUTO_TEST_CASE(part_data)
{
	boost::property_tree::ptree pt;
	pt.put("gen.part.test", true);
	pt.put("gen.other.test", true);
	mg::options_view opts(pt, "part"sv);
	BOOST_CHECK_EQUAL(opts.part_data().get<bool>("test"), true);
	BOOST_CHECK_EQUAL(opts.part_data("other"sv).get<bool>("test"), true);
}
BOOST_AUTO_TEST_CASE(gen_data)
{
	boost::property_tree::ptree pt;
	pt.put("gen.part.test", true);
	pt.put("gen.other.test", true);
	pt.put("gen.lala.test", "string");
	mg::options_view opts(pt, "part"sv);

	BOOST_CHECK_EQUAL(opts.gen_data().get<bool>("part.test"), true);
	BOOST_CHECK_EQUAL(opts.gen_data().get<bool>("other.test"), true);
	BOOST_CHECK_EQUAL(opts.gen_data().get<std::string>("lala.test"), "string");
}
BOOST_AUTO_TEST_SUITE_END() // options_view

BOOST_AUTO_TEST_SUITE(wrong_generation)
BOOST_AUTO_TEST_CASE(no_data)
{
	auto data_gen = std::make_shared<fake_data_gen>();
	data_gen->gen_data = false;

	auto provider = std::make_shared<provider_mock>();
	MOCK_EXPECT( provider->json_jinja ).exactly(0);
	MOCK_EXPECT( provider->parser ).exactly(1).with( "interface"sv ).returns( std::make_shared<fake_target>() );
	MOCK_EXPECT( provider->generator ).exactly(1).with( "cpp"sv ).returns( data_gen );

	mg::generator gen(provider, u8"some/path");
	auto& opts = gen.options();
	opts.put("gen.def.target", "cpp") ;
	opts.put("gen.def.parser", "interface") ;
	opts.put("gen.def.output", "def.hpp") ;
	opts.put("gen.def.input", "definitions.hpp.jinja") ;
	opts.put("gen.def.test", "test_data") ;
	BOOST_CHECK_THROW( gen.generate("some_dir"), modegen::errors::error );
}
BOOST_AUTO_TEST_SUITE_END() // wrong_generation
