/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE cmake

#include <boost/test/unit_test.hpp>
#include <turtle/mock.hpp>

#include "errors.h"
#include "generation/cmake.hpp"
#include "generation/output_info.hpp"
#include "generation/part_descriptor.hpp"

using namespace std::literals;
namespace mg = modegen::generation;

MOCK_BASE_CLASS( part_desc_mock, modegen::generation::part_descriptor )
{
	MOCK_METHOD( part_name, 0 )
	MOCK_METHOD( file_name, 0 )
	MOCK_METHOD( opts, 0 )
	MOCK_METHOD( need_output, 0 )
	MOCK_METHOD( next, 0 )
	MOCK_METHOD( reset, 0 )
	MOCK_METHOD( idl_input, 0 )
	MOCK_METHOD( data_input, 0 )
};

BOOST_AUTO_TEST_CASE(add_library)
{
	auto opts = std::make_shared<mg::options::container>();
	opts->raw().put("gen.cmake.project", "proj");
	opts->raw().put("gen.cmake.libraries.interface.part", "some_part");
	opts->raw().put("gen.cmake.libraries.interface.file", "some_file.cpp");
	//opts->raw().put("gen.some_part.output", "part_file.cpp");
	mg::options::view opts_view(opts, "cmake"s);

	auto cmake_pg = std::make_unique<part_desc_mock>();
	MOCK_EXPECT( cmake_pg->opts ).once().returns( opts_view );
	MOCK_EXPECT( cmake_pg->part_name ).returns( "cmake" );
	MOCK_EXPECT( cmake_pg->reset ).returns( cmake_pg.get() );

	auto some_pg = std::make_unique<part_desc_mock>();
	MOCK_EXPECT( some_pg->part_name ).returns( "some_part" );
	MOCK_EXPECT( some_pg->next ).returns( false );
	MOCK_EXPECT( some_pg->file_name ).returns( "part_file.cpp" );
	MOCK_EXPECT( some_pg->reset ).returns( some_pg.get() );

	mg::output_info oi;
	oi.add_part(std::move(cmake_pg)).add_part(std::move(some_pg)).next();


	mg::cmake cm;
	auto data = cm.jsoned_data(oi);
	BOOST_CHECK_EQUAL(data["libraries"].size(), 1);
	for(auto& [k,v]:data["libraries"].items()) BOOST_CHECK_EQUAL(k, "interface"s);

	BOOST_REQUIRE(data["libraries"]["interface"]["files"].is_array());
	BOOST_REQUIRE_EQUAL(data["libraries"]["interface"]["files"].size(), 2);
	BOOST_CHECK_EQUAL(data["libraries"]["interface"]["files"][0], "part_file.cpp");
	BOOST_CHECK_EQUAL(data["libraries"]["interface"]["files"][1], "some_file.cpp");
}

BOOST_AUTO_TEST_CASE(multioutputs)
{
	auto opts=std::make_shared<mg::options::container>();
	opts->raw().put("gen.cmake.project", "proj");
	opts->raw().put("gen.cmake.libraries.interface.part", "some_part");
	opts->raw().put("", "");
	mg::options::view opts_view(opts, "cmake"s);

	auto cmake_pg = std::make_unique<part_desc_mock>();
	MOCK_EXPECT( cmake_pg->opts ).once().returns( opts_view );
	MOCK_EXPECT( cmake_pg->reset ).once().returns( cmake_pg.get() );
	MOCK_EXPECT( cmake_pg->part_name ).returns( "cmake" );
	MOCK_EXPECT( cmake_pg->file_name ).returns( "cm_fn" );
	MOCK_EXPECT( cmake_pg->next ).returns( false );

	mock::sequence seq_next, seq_fname;
	auto some_pg = std::make_unique<part_desc_mock>();
	MOCK_EXPECT( some_pg->reset ).once().returns( some_pg.get() );
	MOCK_EXPECT( some_pg->part_name ).exactly(1).returns( "some_part" );
	MOCK_EXPECT( some_pg->file_name ).once().in(seq_fname).returns( "sm_fn1.txt" );
	MOCK_EXPECT( some_pg->file_name ).once().in(seq_fname).returns( "sm_fn2.txt" );
	MOCK_EXPECT( some_pg->next ).once().in(seq_next).returns( true );
	MOCK_EXPECT( some_pg->next ).once().in(seq_next).returns( false );

	mg::output_info oi;
	oi.add_part(std::move(cmake_pg)).add_part(std::move(some_pg)).next();

	mg::cmake cm;
	auto data = cm.jsoned_data(oi);
	BOOST_REQUIRE(data["libraries"]["interface"]["files"].is_array());
	BOOST_REQUIRE_EQUAL(data["libraries"]["interface"]["files"].size(), 2);
	BOOST_REQUIRE_EQUAL(data["libraries"]["interface"]["files"][0], "sm_fn1.txt");
	BOOST_REQUIRE_EQUAL(data["libraries"]["interface"]["files"][1], "sm_fn2.txt");
}

