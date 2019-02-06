#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE modegen_parser

#include <boost/test/unit_test.hpp>
#include <turtle/mock.hpp>

#include "generation/common.hpp"
#include "generation/provider.hpp"

using namespace std::literals;
namespace mg = modegen::generation;

MOCK_BASE_CLASS( provider_mock, modegen::generation::provider )
{
	MOCK_METHOD( json_jinja, 3 )
};

BOOST_AUTO_TEST_CASE(common_generation)
{
	// tests normal using

	bool cout = true;
	auto file_data_checker = [&cout](const cppjson::value& data, const std::filesystem::path& tmpl, const std::optional<std::filesystem::path>& out) {
			if(cout) BOOST_CHECK( !out );
			else {
				BOOST_REQUIRE( out );
				BOOST_CHECK_EQUAL( "some_dir" / (*out), "def.hpp" );
			}

			BOOST_CHECK_EQUAL( tmpl, "definitions.hpp.jinja" );
			return true;
		};

	auto provider = std::make_shared<provider_mock>();
	MOCK_EXPECT( provider->json_jinja ).exactly(2).with( file_data_checker ) ;

	mg::generator gen(provider, u8"some/path");
	auto& opts = gen.options();
	opts.put("gen.def.output", "def.hpp") ;
	opts.put("gen.def.input", "definitions.hpp.jinja") ;
	gen.generate_stdout("def");
	gen.generate("some_dir");
	cout = false;
}

