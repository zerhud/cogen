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

using namespace std::literals;
namespace md = modegen::parser::data_tree;

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

