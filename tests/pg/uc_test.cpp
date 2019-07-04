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

#include "pg/generator.hpp"

namespace mpg = modegen::pg;
namespace mpo = modegen::pg::options;

BOOST_AUTO_TEST_CASE(without_build)
{
	mpg::generator g(nullptr,nullptr);
	BOOST_CHECK_EQUAL(g.parts().count(), 0);
}

BOOST_AUTO_TEST_SUITE(all_to_one)
BOOST_AUTO_TEST_SUITE_END() // all_to_one

