#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE modegen_parser

#include <boost/test/unit_test.hpp>

#include "generation/common.hpp"

using namespace std::literals;
namespace mg = modegen::generation;

BOOST_AUTO_TEST_CASE(common_generation)
{
	// tests normal using
	mg::generator gen(nullptr, u8"some/path");
	gen.options()
		.put("gen.def.output", "def.hpp")
		.put("gen.def.input", "definitions.hpp.jinja")
		;
}

