/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE outputs info

#include <boost/test/unit_test.hpp>
#include "generation/output_info.hpp"
#include "generation/part_descriptor.hpp"
#include "errors.h"

using namespace std::literals;
namespace mg = modegen::generation;

struct simple_part_desc : mg::part_descriptor {
	bool& dtored ;

	simple_part_desc(bool* d) : dtored(*d) {dtored = false;}
	~simple_part_desc() override {
		dtored = true;
	}

	std::string part_name() const override {return "part"s;}
	std::string file_name() const override {return ""s;}
	const mg::options::view& opts() const override {throw 1;}
	bool need_output() const override {return false;}
	bool next() override {return false;}

	std::vector<modegen::parser::interface::module> idl_input() const override { return {}; }
	boost::property_tree::ptree data_input() const override { return boost::property_tree::ptree{}; }
};

BOOST_AUTO_TEST_CASE(acceess)
{
	bool dtored = false;

	mg::output_info outputs;
	outputs.add_part(std::make_unique<simple_part_desc>(&dtored));

	BOOST_CHECK( !dtored );
	BOOST_CHECK( !outputs.empty() );

	auto list = outputs.parts();
	BOOST_CHECK( !dtored );
	BOOST_REQUIRE_EQUAL(list.size(), 1);
	BOOST_CHECK(dynamic_cast<simple_part_desc*>(list[0]));
	BOOST_CHECK( !dtored );

	outputs.select("part"sv);
	BOOST_CHECK( !dtored );

	BOOST_CHECK(dynamic_cast<simple_part_desc*>(outputs.current_part()));
	BOOST_CHECK( !dtored );

	BOOST_CHECK( !outputs.next() );
	BOOST_CHECK( !dtored );
}

BOOST_AUTO_TEST_SUITE(iterate)
BOOST_AUTO_TEST_CASE(one)
{
	bool dtored = false;
	mg::output_info outputs;
	outputs.add_part(std::make_unique<simple_part_desc>(&dtored));
	BOOST_CHECK_THROW( outputs.current_part(), modegen::errors::error );
	BOOST_CHECK( outputs.next() );
	BOOST_CHECK( outputs.current_part() );
	BOOST_CHECK( !outputs.next() );
	BOOST_CHECK( outputs.current_part() );
}
BOOST_AUTO_TEST_CASE(tree)
{
	bool dtored = false;
	mg::output_info outputs;
	outputs.add_part(std::make_unique<simple_part_desc>(&dtored));
	outputs.add_part(std::make_unique<simple_part_desc>(&dtored));

	BOOST_CHECK_THROW( outputs.current_part(), modegen::errors::error );
	BOOST_CHECK( outputs.next() );
	BOOST_CHECK( outputs.current_part() );
	BOOST_CHECK( outputs.next() );
	BOOST_CHECK( outputs.current_part() );
	BOOST_CHECK( !outputs.next() );
	BOOST_CHECK( outputs.current_part() );
}
BOOST_AUTO_TEST_SUITE_END() // iterate
