/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ic_ptsetts

#include <boost/test/included/unit_test.hpp>
#include <common_utils/tests/mocks.hpp>

#include "mocks.hpp"
#include "ic/ptsetts.hpp"

using namespace std::literals;
using ic_ptsetts = mdg::ic::ptsetts;
using boost::property_tree::ptree;


BOOST_AUTO_TEST_SUITE(input_configurator)
BOOST_AUTO_TEST_SUITE(ptsetts)
BOOST_AUTO_TEST_CASE(map_tmpl)
{
	ptree setts;
	setts.put("part.a.file", "v_file");
	setts.put("part.a.tmpl", "v_tmpl");
	setts.put("part.a.inc_part", "b");
	setts.put("part.a.inc_part", "c");
	setts.put("part.b.tmpl", "v_tmpl2");
	ic_ptsetts obj(setts);
	BOOST_TEST(obj.parts().size() == 2);
	modegen::ic::gen_context a_setts;
	obj.part_setts("a"sv, a_setts);
	BOOST_TEST(a_setts.map_tmpl == "v_file");
	BOOST_TEST(a_setts.tmpl_file == "v_tmpl");
	BOOST_TEST(a_setts.links.size() == 2);
	//BOOST_TEST(a_setts.links.at(0) == "a");
	//BOOST_TEST(a_setts.links.at(1) == "c");
	BOOST_CHECK_THROW(obj.part_setts("b"sv, a_setts), std::exception);
}
BOOST_AUTO_TEST_SUITE_END() // ptsetts
BOOST_AUTO_TEST_SUITE_END() // input_configuration
