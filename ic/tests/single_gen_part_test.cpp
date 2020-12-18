/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ic_single_gen_part

#include <boost/test/included/unit_test.hpp>
#include <common_utils/tests/mocks.hpp>

#include "mocks.hpp"
#include "generation_part.hpp"

BOOST_AUTO_TEST_SUITE(ic)
using modegen::ic::single_gen_part;
BOOST_AUTO_TEST_CASE(main_rules)
{
	auto prov = std::make_shared<icmocks::provider>();
	boost::property_tree::ptree setts;
	setts.put("parts.one.file", "file.cpp");
	setts.put("parts.one.gen", "cpp");
	single_gen_part sg(prov);
	//sg(setts, input);
}
BOOST_AUTO_TEST_SUITE_END() // ic

