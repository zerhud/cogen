/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE data_tree_loader

#include <sstream>
#include <boost/test/unit_test.hpp>

#include "errors.h"
#include "parser/data_tree/loader.hpp"
#include "parser/data_tree/ptree_merge.hpp"

using namespace std::literals;
namespace md = modegen::parser::data_tree;
namespace pt = boost::property_tree;
using ostr = boost::optional<std::string>;

BOOST_AUTO_TEST_CASE(merge)
{
	std::stringstream src1, src2;
	src1 << "{\"one\":\"one_v\"}"s;
	src2 << "{\"two\":\"two_v\"}"s;

	md::loader_impl ldr(md::loader_impl::data_format::json);
	ldr.load(src1, "testf1"s);
	ldr.load(src2, "testf2"s);

	BOOST_REQUIRE(ldr.boost_ptree().get_optional<std::string>("two"s));
	BOOST_REQUIRE(ldr.boost_ptree().get_optional<std::string>("one"s));
	BOOST_CHECK_EQUAL(ldr.boost_ptree().get<std::string>("one"s), "one_v"s);
	BOOST_CHECK_EQUAL(ldr.boost_ptree().get<std::string>("two"s), "two_v"s);
}

BOOST_AUTO_TEST_SUITE(merger)
BOOST_AUTO_TEST_CASE(add)
{
	pt::ptree basic, adding;
	basic.put("leaf"s, "lv"s);
	basic.put("sub.leaf"s, "slv"s);
	basic.put("sub2.leaf"s, "s2lv"s);
	basic.put("sub.sub.leaf", "sslv"s);
	basic.put("tadd.add"s, "1");
	adding.put("add"s, "alv"s);
	adding.put("sub.add"s, "aslv");
	adding.put("addsub.add"s, "as2lv");
	adding.put("tadd.add"s, "2");

	pt::merger mr(std::move(basic));
	pt::ptree result = mr.add(adding);
	BOOST_CHECK(!result.empty());
	BOOST_CHECK_EQUAL(result.get_optional<std::string>("leaf"s), ostr("lv"s));
	BOOST_CHECK_EQUAL(result.get_optional<std::string>("add"s), ostr("alv"s));
	BOOST_CHECK_EQUAL(result.get_optional<std::string>("sub.leaf"s), ostr("slv"s));
	BOOST_CHECK_EQUAL(result.get_optional<std::string>("sub.sub.leaf"s), ostr("sslv"s));
	BOOST_CHECK_EQUAL(result.get_optional<std::string>("sub2.leaf"s), ostr("s2lv"s));
	BOOST_CHECK_EQUAL(result.get_optional<std::string>("sub.add"s), ostr("aslv"s));
	BOOST_CHECK_EQUAL(result.get_optional<std::string>("addsub.add"s), ostr("as2lv"s));
}
BOOST_AUTO_TEST_SUITE_END() // merger
