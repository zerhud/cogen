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
BOOST_AUTO_TEST_CASE(ptarray)
{
	boost::property_tree::ptree pt;
	pt.put("a.b", "ab");
	pt.add("a.c", "ac1");
	pt.add("a.c", "ac2");
	pt.add("a.c", "ac3");
	std::size_t count_b = 0;
	std::size_t count_c = 0;
	for(auto& child:pt.get_child("a")) {
		if(child.first == "b") ++count_b;
		else if(child.first == "c") ++count_c;
	}
	BOOST_TEST(count_b==1);
	BOOST_TEST(count_c==3);
}
BOOST_AUTO_TEST_CASE(map_tmpl)
{
	ptree setts;
	setts.put("part.a.file", "v_file");
	setts.put("part.a.tmpl", "v_tmpl");
	setts.add("part.a.inc_part", "b");
	setts.add("part.a.inc_part", "c");
	setts.put("part.b.tmpl", "v_tmpl2");
	ic_ptsetts obj(setts);
	BOOST_TEST(obj.parts().size() == 2);
	modegen::ic::gen_context a_setts;
	a_setts.links.emplace_back("a");
	obj.part_setts("a"sv, a_setts);
	BOOST_TEST(a_setts.map_tmpl == "v_file");
	BOOST_TEST(a_setts.tmpl_file == "v_tmpl");
	BOOST_TEST_REQUIRE(a_setts.links.size() == 2);
	BOOST_TEST(a_setts.links.at(0) == "b");
	BOOST_TEST(a_setts.links.at(1) == "c");
	BOOST_CHECK_THROW(obj.part_setts("b"sv, a_setts), std::exception);
}
BOOST_AUTO_TEST_SUITE_END() // ptsetts
BOOST_AUTO_TEST_SUITE_END() // input_configuration
