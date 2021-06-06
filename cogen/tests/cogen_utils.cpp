/*************************************************************************
 * Copyright © 2021 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ic_ptsetts

#include <regex>
#include <boost/test/included/unit_test.hpp>
#include "src/path_config.hpp"

//bool check_by_regex(std::string_view str, std::reg

BOOST_AUTO_TEST_SUITE(cogen_utils)
BOOST_AUTO_TEST_SUITE(pathes)
BOOST_AUTO_TEST_CASE(libraries)
{
	cogen::path_config pc(std::filesystem::current_path());
	std::stringstream out;
	pc.print_pathes(out, cogen::avaible_pathes::libraries);
	BOOST_CHECK(std::regex_search(out.str(), std::regex("\"\\.\"")));
	BOOST_CHECK(std::regex_search(out.str(), std::regex("/etc/[^/]+/lib\"")));
}
BOOST_AUTO_TEST_CASE(generators)
{
	cogen::path_config pc(std::filesystem::current_path());
	std::stringstream out;
	pc.print_pathes(out, cogen::avaible_pathes::generators);
	BOOST_CHECK(std::regex_search(out.str(), std::regex("\"\\.\"")));
	BOOST_CHECK(std::regex_search(out.str(), std::regex("/etc/[^/]+/generators\"")));
}
BOOST_AUTO_TEST_CASE(input)
{
	cogen::path_config pc(std::filesystem::current_path());
	std::stringstream out;
	pc.print_pathes(out, cogen::avaible_pathes::input);
	BOOST_CHECK(std::regex_search(out.str(), std::regex("\"\\.\"")));
	BOOST_CHECK(std::regex_search(out.str(), std::regex("/etc/[^/]+/examples\"")));
}
BOOST_AUTO_TEST_SUITE_END() // pathes
BOOST_AUTO_TEST_SUITE_END() // cogen_utils
