#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE modegen_parser

#include <boost/test/unit_test.hpp>

#include "parser/grammar.hpp"
#include "generator.hpp"

using namespace std::literals;

BOOST_AUTO_TEST_SUITE(pipe_fileters)
BOOST_AUTO_TEST_CASE(select)
{
	//auto ph = modegen::parse("module mod v1.0: enum e{a b} void some();"sv);

	//modegen::generator_request req;
	//req.data = ph.mods;
	//req.selector.sel = modegen::module_content_selector::enumeration;

	//BOOST_REQUIRE_EQUAL(req.data.size(), 1);
	//BOOST_CHECK_EQUAL(req.data[0].content.size(), 2);
	//req | modegen::generation::select;
	//BOOST_CHECK_EQUAL(req.data[0].content.size(), 1);
}
BOOST_AUTO_TEST_SUITE_END() // pipe_filters
