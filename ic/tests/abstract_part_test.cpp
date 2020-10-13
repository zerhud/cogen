/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ic_abstract_part

#include <boost/test/included/unit_test.hpp>

#include "abstract_part.hpp"

using namespace std::literals;

BOOST_AUTO_TEST_SUITE(input_configurator)
BOOST_AUTO_TEST_SUITE(abstract_part)
using abs_part = modegen::ic::abstract::part;
BOOST_AUTO_TEST_CASE(getters)
{
	abs_part p(10, "name", nullptr);
	BOOST_TEST(p.id()==10);
	BOOST_TEST(p.name()=="name"s);
}
BOOST_AUTO_TEST_SUITE(map_to)
struct fixture {
	abs_part part;
	fixture() : part(10, "name", nullptr)
	{
	}
};
BOOST_FIXTURE_TEST_CASE(no_vars, fixture)
{
	part.map_to("no_vars"s);
	//BOOST_TEST(part.outputs().size()==1);
}
BOOST_AUTO_TEST_SUITE_END() // map_to
BOOST_AUTO_TEST_SUITE_END() // abstract_part
BOOST_AUTO_TEST_SUITE_END() // input_configurator

