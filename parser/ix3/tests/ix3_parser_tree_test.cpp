/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE parser_tree

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parser.hpp"
#include "common_utils/input/tree.hpp"



BOOST_AUTO_TEST_CASE(tree)
{
	ix3::parser parser ;
	std::stringstream str ;
	str << "module mod v1.0:" << std::endl << "string foo();" ;

	parser.parse(str, "file.txt") ;

	auto result = parser.result() ;
	std::cout << std::endl << result.size() << std::endl ;
}