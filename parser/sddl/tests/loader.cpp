/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE sddl_loader

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <filesystem>

#include "utils/generic_dsl.hpp"

extern "C" gen_utils::generic_sdl_factory* create_dsl();

BOOST_AUTO_TEST_SUITE(sddl)
BOOST_AUTO_TEST_SUITE(loader)

BOOST_AUTO_TEST_CASE(create_dsl_returns_rigth_loader)
{
	auto factory = create_dsl();
	BOOST_REQUIRE(factory!=nullptr);
	auto langs = factory->languages([](auto){return "";});
	BOOST_TEST(langs.size() == 1);
	BOOST_TEST(langs[0]->name() == "sddl");
}

BOOST_AUTO_TEST_SUITE_END() // sddl
BOOST_AUTO_TEST_SUITE_END() // loader
