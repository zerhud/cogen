/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE split_by_ver

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "common_utils/input/tree.hpp"
#include "common_utils/input/input.hpp"
#include "common_utils/input/split_by_ver.hpp"
#include "mocks.hpp"

using namespace std::literals;

std::pmr::string operator "" _s (const char* d, std::size_t l)
{
	return std::pmr::string(d, l);
}

BOOST_AUTO_TEST_SUITE(input)
BOOST_AUTO_TEST_SUITE(version_splitter)
using gen_utils::split_by_ver;
using gen_utils_mocks::make_node;
using gen_utils_mocks::trees_fixture;
BOOST_FIXTURE_TEST_CASE(no_vers, trees_fixture)
{
	std::pmr::vector<gen_utils::tree> r =
		split_by_ver{}(t1());
	BOOST_TEST(r.at(0).children(r.at(0).root()).size() == 0);
	t1().add(t1().root(), make_node(1));
	t1().add(t1().root(), make_node(2));
	r = split_by_ver{}(t1());
	BOOST_TEST(r.size() == 2);
	BOOST_TEST(r.at(0).children(r.at(0).root()).size() == 1);
	BOOST_TEST(r.at(1).children(r.at(0).root()).size() == 1);
}
BOOST_AUTO_TEST_SUITE_END() // version_splitter
BOOST_AUTO_TEST_SUITE_END() // input
