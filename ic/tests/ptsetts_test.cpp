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
	setts.put("part.c.file", "v_file");
	setts.put("part.c.tmpl", "v_tmpl");
	setts.put("part.c.split_by_version", true);
	setts.put("part.c.naming", "title_case");
	setts.put("part.c.self_inc", true);
	ic_ptsetts obj(setts);
	BOOST_TEST(obj.parts().size() == 3);
	mic::gen_context a_setts;
	a_setts.cfg_part = obj.part_setts("a"sv);
	BOOST_TEST(a_setts.cfg_part.map_tmpl == "v_file");
	BOOST_TEST(a_setts.cfg_part.tmpl_file == "v_tmpl");
	BOOST_TEST(a_setts.cfg_part.split_by_version == false);
	BOOST_TEST_REQUIRE(a_setts.cfg_part.links.size() == 2);
	BOOST_TEST(a_setts.cfg_part.links.at(0) == "b");
	BOOST_TEST(a_setts.cfg_part.links.at(1) == "c");
	BOOST_TEST(a_setts.cfg_part.include_own_part == false);
	BOOST_CHECK(a_setts.cfg_part.compilation.naming.size() == 1);
	BOOST_CHECK_THROW(obj.part_setts("b"sv), std::exception);
	BOOST_CHECK_NO_THROW(obj.part_setts("c"sv));
	BOOST_TEST(obj.part_setts("c"sv).split_by_version == true);
	BOOST_TEST(obj.part_setts("c"sv).compilation.naming.size() == 1);
	BOOST_CHECK(obj.part_setts("c"sv).compilation.naming.at(0) == gen_utils::name_conversion::title_case);
	BOOST_TEST(obj.part_setts("c"sv).include_own_part == true);
}
BOOST_AUTO_TEST_CASE(generic_ast_tree)
{
	ptree setts;
	setts.put("part.a.file", "f");
	setts.put("part.a.tmpl", "t");
	setts.put("part.a.some", "v");
	ic_ptsetts obj(setts);
	BOOST_TEST(obj.generic_ast("a").data_id() == "ptsetts");
	auto ga = obj.generic_ast("a");
	BOOST_TEST(ga.children(ga.root()).size() == 0);
	gen_utils::compilation_context ctx;
	BOOST_TEST(ga.to_json(ctx) == "{\"file\":\"f\", \"tmpl\":\"t\", \"some\":\"v\"}"_bj);
}
BOOST_AUTO_TEST_CASE(part_src)
{
	ptree setts;
	setts.put("part.a.file", "f");
	setts.put("part.a.tmpl", "t");
	setts.put("part.a.some", "v");
	setts.put("part.a.kuku", "ok");
	ic_ptsetts obj(setts);
	BOOST_TEST(obj.part_src("a").get<std::string>("file") == "f");
	BOOST_TEST(obj.part_src("a").get<std::string>("tmpl") == "t");
	BOOST_TEST(obj.part_src("a").get<std::string>("some") == "v");
	BOOST_TEST(obj.part_src("a").get<std::string>("kuku") == "ok");
}
BOOST_AUTO_TEST_SUITE_END() // ptsetts
BOOST_AUTO_TEST_SUITE_END() // input_configuration
