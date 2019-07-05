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
#include <turtle/mock.hpp>

#include "pg/provider.hpp"
#include "pg/generator.hpp"
#include "pg/options.hpp"
#include "pg/part_descriptor.hpp"

#include "parser/interface/loader.hpp"

namespace mp = modegen::parser;
namespace mpg = modegen::pg;
namespace mpo = modegen::pg::options;
namespace mpi = modegen::parser::interface;

// use cases
// 01. all modules to one file [out_single]
// 02. each module to own flie [out_permod]
// 03. each module to own file in own dir (with same structure) [out_permod]
// 04. cpp one unite file, python module to own file [out_single out_permod]
// 05. one cmake file and library for each module [out_single map_in_library]
// 06. cmake installs python's modules [out_single map_in_install]
// 07. one module depends from other, all to one [out_single map_in_includes]
// 08. A.foo depends from B.bar and C.car. each per file (B has B.foo and B.bar, each per file). [out_perent find_ent]
// 09. module has few thinks inside, each per file [out_perent]
//
// error cases
// 01. part depends from part on other language

MOCK_BASE_CLASS( mock_provider, mpg::provider)
{
	MOCK_METHOD( create_part, 1 )
	MOCK_METHOD( input, 0 )
};

MOCK_BASE_CLASS( mock_part, mpg::part_descriptor )
{
	MOCK_METHOD( lang, 0 )
	MOCK_METHOD( name, 0 )
	MOCK_METHOD( outputs, 1 )
	MOCK_METHOD( build_outputs, 2 )
};

MOCK_BASE_CLASS( mock_iloader, mpi::loader )
{
	void load(std::istream& /*input*/, std::string /*fn*/) override {}
	void load(FS::path /*file*/) override {}

	MOCK_METHOD( result, 0 )
	MOCK_METHOD( finish_loads, 0 )
};

BOOST_AUTO_TEST_SUITE(all_to_one)
BOOST_AUTO_TEST_CASE(one_module)
{
	auto pf = mpi::parse("module m1 v1.1:");
	auto setts = std::make_shared<mpo::container>();
	setts->raw().put("part.fcpp.lang", "cpp");
	setts->raw().put("part.fcpp.file_single", "test.cpp");

	module_part part(output_lang::cpp, "fcpp");
}
BOOST_AUTO_TEST_CASE(two_modules)
{
	auto pf = mpi::parse("module m1 v1.1: module m2 v2.2:");
}
BOOST_AUTO_TEST_SUITE_END() // all_to_one

BOOST_AUTO_TEST_SUITE(permod)
BOOST_AUTO_TEST_CASE(two_modules)
{
	auto pf = mpi::parse("module m1 v1.1: module m2 v2.2:");
}
BOOST_AUTO_TEST_SUITE_END() // permod
