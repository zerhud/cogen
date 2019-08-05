/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE name_conversion

#include <boost/test/unit_test.hpp>

#include "mocks.hpp"
#include "pg/part_algos/module.hpp"

namespace pa = modegen::pg::palgos;
namespace pi = modegen::parser::interface;
using namespace std::literals;

BOOST_AUTO_TEST_SUITE(modules)
BOOST_AUTO_TEST_SUITE(map)
BOOST_AUTO_TEST_SUITE(map_to)
BOOST_AUTO_TEST_CASE(result)
{
	using pgmocks::mapk_to_vec;
	auto pf = pi::parse("module m1 v1.0: module m1 v1.1: module m2 v1.0: module m2 v2.0:"sv);
	auto ildr = std::make_shared<pgmocks::iloader>();
	MOCK_EXPECT(ildr->result).once().returns(pf.mods);

	pa::module_algos ma({ildr});
	auto maps = mapk_to_vec(ma.map_to("test"s));
	BOOST_REQUIRE_EQUAL(maps.size(), 1);
	BOOST_CHECK_EQUAL(maps[0], "test"s);

	maps = mapk_to_vec(ma.map_to("$mod"s));
	BOOST_REQUIRE_EQUAL(maps.size(), 2);
	BOOST_CHECK_EQUAL(maps[0], "m1"s);
	BOOST_CHECK_EQUAL(maps[1], "m2"s);

	maps = mapk_to_vec(ma.map_to("$mod_$va"s));
	BOOST_REQUIRE_EQUAL(maps.size(), 3);
	BOOST_CHECK_EQUAL(maps[0], "m1_1"s);
	BOOST_CHECK_EQUAL(maps[1], "m2_1"s);
	BOOST_CHECK_EQUAL(maps[2], "m2_2"s);

	maps = mapk_to_vec(ma.map_to("$mod_$va_$vi"));
	BOOST_REQUIRE_EQUAL(maps.size(), 4);
	BOOST_CHECK_EQUAL(maps[0], "m1_1_0"s);
	BOOST_CHECK_EQUAL(maps[1], "m1_1_1"s);
	BOOST_CHECK_EQUAL(maps[2], "m2_1_0"s);
	BOOST_CHECK_EQUAL(maps[3], "m2_2_0"s);
}
BOOST_AUTO_TEST_SUITE_END() // map_to
BOOST_AUTO_TEST_SUITE_END() // map
BOOST_AUTO_TEST_SUITE_END() // modules

