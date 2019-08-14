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

#include "pg/langs/cpp/type_converter.hpp"
#include "parser/interface/grammar.hpp"

using namespace std::literals;
namespace mg = modegen::pg;
namespace mi = modegen::parser::interface;

BOOST_AUTO_TEST_CASE(type_converted)
{
	auto ph = mi::parse("module mod v1.0: string foo(list<type> bar);"sv);
	mg::cpp::type_converter tc;
	auto mods = tc(ph.mods);

	BOOST_REQUIRE_EQUAL(mods.size(), 1);
	BOOST_REQUIRE_EQUAL(mods[0].content.size(), 1);

	mi::function& foo = std::get<mi::function>(mods[0].content[0]);
	BOOST_CHECK_EQUAL( foo.return_type.name, "std::string"sv );
	BOOST_REQUIRE_EQUAL( foo.func_params.size(), 1);
	BOOST_CHECK_EQUAL( foo.func_params[0].param_type.name, "std::vector"sv );
	BOOST_REQUIRE_EQUAL( foo.func_params[0].param_type.sub_types.size(), 1);
	BOOST_CHECK_EQUAL( foo.func_params[0].param_type.sub_types[0].name, "type"sv );
}

BOOST_AUTO_TEST_CASE(includes)
{
	auto ph = mi::parse("module mod v1.0: string foo(list<type> bar);"sv);
	mg::cpp::type_converter tc;
	tc(ph.mods);

	auto incs = tc.includes();
	BOOST_REQUIRE_EQUAL(incs.size(), 2);
	BOOST_CHECK_EQUAL(incs[0], "string"sv);
	BOOST_CHECK_EQUAL(incs[1], "vector"sv);
}
