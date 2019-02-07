#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE modegen_parser

#include <boost/test/unit_test.hpp>
#include <turtle/mock.hpp>

#include "generation/common.hpp"
#include "generation/provider.hpp"

using namespace std::literals;
namespace mg = modegen::generation;
namespace fs = std::filesystem;

MOCK_BASE_CLASS( provider_mock, modegen::generation::provider )
{
	MOCK_METHOD( json_jinja, 3 )
	MOCK_METHOD( target_data, 1 )
	MOCK_METHOD( target_generator, 1 )
};

class fake_target : public modegen::parser::loader {
	void load(std::istream& input, std::string fn) override {}
	void load(std::filesystem::path file) override {}
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

			BOOST_CHECK_EQUAL( tmpl, "definitions.hpp.jinja" );
			return true;
		};

	auto provider = std::make_shared<provider_mock>();
	MOCK_EXPECT( provider->json_jinja ).exactly(2).with( result_data_checker ) ;
	MOCK_EXPECT( provider->target_data ).exactly(2).with( "cpp"sv ).returns( std::make_shared<fake_target>() );

	mg::generator gen(provider, u8"some/path");
	auto& opts = gen.options();
	opts.put("gen.def.target", "cpp") ;
	opts.put("gen.def.output", "def.hpp") ;
	opts.put("gen.def.input", "definitions.hpp.jinja") ;

	// first calls
	gen.generate_stdout("def");

	// second calls
	cout = false;
	gen.generate("some_dir");
}

