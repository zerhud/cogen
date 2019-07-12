/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE module use cases

#include <boost/test/unit_test.hpp>

#include "mocks.hpp"
#include "pg/part_descriptor.hpp"
#include "pg/part_manager.hpp"
#include "pg/exceptions.hpp"

namespace mpg = modegen::pg;

BOOST_AUTO_TEST_CASE(list_require_part_by_name)
{
	auto part1 = std::make_shared<pgmocks::part>();
	auto part2 = std::make_shared<pgmocks::part>();
	MOCK_EXPECT( part1->name ).returns("part1");
	MOCK_EXPECT( part2->name ).returns("part2");

	mpg::part_manager pm;
	BOOST_CHECK_NO_THROW( pm.add(part1).add(part2) );
	BOOST_CHECK(pm.require("part2"));
	BOOST_CHECK_EQUAL(pm.require("part1")->name(), "part1");
	BOOST_CHECK_THROW(pm.require("no_part"), mpg::errors::error);

	BOOST_REQUIRE_EQUAL(pm.list().size(), 2);
	BOOST_CHECK_EQUAL(pm.list()[0]->name(), "part1");
	BOOST_CHECK_EQUAL(pm.list()[1]->name(), "part2");
}

BOOST_AUTO_TEST_CASE(list_outputs)
{
	auto part1 = std::make_shared<pgmocks::part>();
	auto part2 = std::make_shared<pgmocks::part>();
	auto part1_out1 = std::make_shared<pgmocks::part_output>();
	auto part1_out2 = std::make_shared<pgmocks::part_output>();
	auto part2_out1 = std::make_shared<pgmocks::part_output>();
	MOCK_EXPECT( part1->name ).returns("part1");
	MOCK_EXPECT( part2->name ).returns("part2");
	MOCK_EXPECT( part2->outputs ).once().returns( std::vector<mpg::output_descriptor_ptr>{part2_out1} );
	MOCK_EXPECT( part1->outputs ).once().returns( std::vector<mpg::output_descriptor_ptr>{part1_out1, part1_out2} );

	mpg::part_manager pm;
	BOOST_CHECK_EQUAL(pm.list_output().size(), 0);
	BOOST_CHECK_NO_THROW( pm.add(part1).add(part2) );
	auto olist = pm.list_output();
	BOOST_CHECK_EQUAL(olist.size(), 3);
	auto pos = std::find(olist.begin(),olist.end(), part1_out1);
	BOOST_CHECK(pos!=olist.end());
	pos = std::find(olist.begin(),olist.end(), part1_out2);
	BOOST_CHECK(pos!=olist.end());
	pos = std::find(olist.begin(),olist.end(), part2_out1);
	BOOST_CHECK(pos!=olist.end());
}

BOOST_AUTO_TEST_CASE(list_outputs_with_nullptr)
{
	auto part1 = std::make_shared<pgmocks::part>();
	auto part2 = std::make_shared<pgmocks::part>();
	auto part1_out1 = std::make_shared<pgmocks::part_output>();
	auto part1_out2 = std::make_shared<pgmocks::part_output>();
	MOCK_EXPECT( part1->name ).returns("part1");
	MOCK_EXPECT( part2->name ).returns("part2");
	MOCK_EXPECT( part2->outputs ).once().returns( std::vector<mpg::output_descriptor_ptr>{nullptr} );
	MOCK_EXPECT( part1->outputs ).once().returns( std::vector<mpg::output_descriptor_ptr>{part1_out1, part1_out2} );

	mpg::part_manager pm;
	BOOST_CHECK_NO_THROW( pm.add(part1).add(part2) );
	BOOST_CHECK_THROW(pm.list_output(), mpg::errors::error);
}
