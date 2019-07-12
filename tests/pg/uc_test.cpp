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

#include "pg/generator.hpp"
#include "pg/options.hpp"
#include "pg/info_part.hpp"

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


BOOST_AUTO_TEST_SUITE(all_to_one)
BOOST_AUTO_TEST_CASE(one_module)
{
	auto pf = mpi::parse("module m1 v1.1: int foo();");
	auto setts = std::make_shared<mpo::container>();
	setts->raw().put("part.fcp1.lang", "cpp");
	setts->raw().put("part.fcp1.file_single", "test1.cpp");
	setts->raw().put("part.fcp2.lang", "cpp");
	setts->raw().put("part.fcp2.file_single", "test2.cpp");

	mpg::part_manager pm;
	pm.add(std::make_shared<mpg::info_part>(mpo::part_view(setts,"fcp1")));
	BOOST_CHECK_EQUAL(pm.require("fcp1")->name(), "fcp1");
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

