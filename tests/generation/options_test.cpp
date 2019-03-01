/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE options

#include <boost/property_tree/json_parser.hpp>
#include <boost/test/unit_test.hpp>
#include <turtle/mock.hpp>

#include "generation/options.hpp"

using namespace std::literals;
namespace mo = modegen::generation::options;

BOOST_AUTO_TEST_SUITE(part)
BOOST_AUTO_TEST_CASE(defaults)
{
	mo::container cnt;
	cnt.raw().put("gen.part.input", "pin");
	cnt.raw().put("defaults.output", "pout");
	cnt.raw().put("file_gen.cpp.naming", "n");
	BOOST_CHECK_EQUAL(cnt.get<std::string>(mo::part_option::input, "part", ""), "pin");
	BOOST_CHECK_EQUAL(cnt.get<std::string>(mo::part_option::output, "part", ""), "pout");

	mo::container_ptr cnt_ptr(&cnt, [](mo::container*){});
	mo::view v(cnt_ptr, "part"sv);
	BOOST_CHECK_EQUAL(v.get<std::string>(mo::part_option::input), "pin");
	BOOST_CHECK_EQUAL(v.get<std::string>(mo::part_option::output), "pout");
	cnt.raw().put("gen.other.input", "oin");
	v.part("other"s);
	BOOST_CHECK_EQUAL(v.get<std::string>(mo::part_option::input), "oin");

	v.file_generator("cpp"sv);
	auto pt = v.get_subset(mo::subsetts::file_generator);
	BOOST_CHECK_EQUAL(v.get_subset(mo::subsetts::file_generator).get<std::string>("naming"), "n");
}
BOOST_AUTO_TEST_CASE(opt)
{
	mo::container cnt;
	cnt.raw().put("gen.part.input", "pin");
	cnt.raw().put("defaults.output", "pout");
	auto opt_input = cnt.get_opt<std::string>(mo::part_option::input, "part", "");
	BOOST_REQUIRE(opt_input.has_value());
	BOOST_CHECK_EQUAL(*opt_input, "pin"s);
	BOOST_CHECK( !cnt.get_opt<std::string>(mo::part_option::input, "other", "") );

	auto opt_output = cnt.get_opt<std::string>(mo::part_option::output, "part", "");
	BOOST_REQUIRE(opt_output);
	BOOST_CHECK_EQUAL(*opt_output, "pout"s);

	mo::container_ptr cnt_ptr(&cnt, [](mo::container*){});
	mo::view v(cnt_ptr, "part"sv);
	BOOST_REQUIRE(v.get_opt<std::string>(mo::part_option::input));
	BOOST_CHECK_EQUAL( *v.get_opt<std::string>(mo::part_option::input), "pin");
	BOOST_CHECK( !v.get_opt<std::string>(mo::part_option::file_generator) );
}
BOOST_AUTO_TEST_CASE(vec)
{
	mo::container cnt;
	cnt.raw().add("gen.part.input", "pin1");
	cnt.raw().add("gen.part.input", "pin2");
	cnt.raw().add("gen.part.input", "pin3");
	auto vec = cnt.get_vec<std::string>(mo::part_option::input, "part", "");
	BOOST_REQUIRE_EQUAL(vec.size(), 3);
	BOOST_CHECK_EQUAL(vec[0], "pin1");
	BOOST_CHECK_EQUAL(vec[1], "pin2");
	BOOST_CHECK_EQUAL(vec[2], "pin3");
	BOOST_CHECK_EQUAL(cnt.get_vec<int>(mo::part_option::output, "part", "").size(), 0);

	mo::container_ptr cnt_ptr(&cnt, [](mo::container*){});
	mo::view v(cnt_ptr, "part"sv);
	vec = v.get_vec<std::string>(mo::part_option::input);
	BOOST_REQUIRE_EQUAL(vec.size(), 3);
	BOOST_CHECK_EQUAL(vec[0], "pin1");
	BOOST_CHECK_EQUAL(vec[1], "pin2");
	BOOST_CHECK_EQUAL(vec[2], "pin3");
	BOOST_CHECK_EQUAL(v.get_vec<int>(mo::part_option::output).size(), 0);
}
BOOST_AUTO_TEST_SUITE_END() // part

BOOST_AUTO_TEST_CASE(subset)
{
	mo::container cnt;
	cnt.raw().put("file_gen.cpp.opt", "cppopt");
	BOOST_CHECK( cnt.get_subset(mo::subsetts::file_generator, "", "ee"s).empty() );
	BOOST_CHECK_EQUAL(cnt.get_subset(mo::subsetts::file_generator, "", "cpp"s).get<std::string>("opt"s), "cppopt"s);

	mo::container_ptr cnt_ptr(&cnt, [](mo::container*){});
	mo::view v(cnt_ptr, "part"sv);
	v.file_generator("cpp"sv);
	BOOST_CHECK_EQUAL(v.get_subset(mo::subsetts::file_generator).get<std::string>("opt"s), "cppopt"s);
}
