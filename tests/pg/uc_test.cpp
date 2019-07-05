/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE common_generator

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
// 01. all modules to one file
// 02. each module to own flie
// 03. each module to own file in own dir (with same structure)
// 04. cpp one unite file, python module to own file
// 05. one cmake file and library for each module
// 06. cmake installs python's modules
// 07. one module depends from other, all to one
// 08. module A depends form B and C, each per file
// 09. A.foo depends from B.bar and C.car. each per file (b has B.foo and B.booz, each per file).
// 10. module has few thinks inside, each per file

MOCK_BASE_CLASS( mock_provider, mpg::provider)
{
	MOCK_METHOD( create_part, 1 )
	MOCK_METHOD( input, 0 )
};

MOCK_BASE_CLASS( mock_part, mpg::part_descriptor )
{
	MOCK_METHOD( name, 0 )
	MOCK_METHOD( outputs, 0 )
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
}
BOOST_AUTO_TEST_SUITE_END() // all_to_one
