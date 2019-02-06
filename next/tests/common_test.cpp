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
	MOCK_METHOD( json_jinja, 2 )
};

BOOST_AUTO_TEST_CASE(common_generation)
{
	// tests normal using

	auto provider = std::make_shared<provider_mock>();
	MOCK_EXPECT( provider->json_jinja ).once() ;

	mg::generator gen(provider, u8"some/path");
	gen.options()
		.put("gen.def.output", "def.hpp")
		.put("gen.def.input", "definitions.hpp.jinja")
		;
	gen.generate_stdout("def");
}

