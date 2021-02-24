/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE standard_types_parser

#include <iomanip>
#include <boost/test/included/unit_test.hpp>
#include <common_utils/tests/mocks.hpp>

#include "loader.hpp"
#include "nodes.hpp"

using namespace std::literals;
using boost::property_tree::ptree;

BOOST_AUTO_TEST_SUITE(library)
BOOST_AUTO_TEST_SUITE(builder_system)
BOOST_AUTO_TEST_CASE(nodes)
{
}
BOOST_AUTO_TEST_SUITE_END() // builder_system
BOOST_AUTO_TEST_SUITE_END() // library
