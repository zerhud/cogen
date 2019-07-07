/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE part_descriptor

#include <boost/test/unit_test.hpp>

#include "mocks.hpp"
#include "pg/module_part.hpp"

namespace mpg = modegen::pg;
using namespace std::literals;

BOOST_AUTO_TEST_SUITE(module_part)
BOOST_AUTO_TEST_CASE(constants)
{
	auto prov = std::make_shared<pgmocks::mock_provider>();
	auto setts = std::make_shared<mpg::options::container>();
	setts->raw().put("part.fcpp.output_lang", "cpp");
	mpg::module_part pd(prov, mpg::options::part_view(setts, "fcpp"sv));
	BOOST_CHECK_EQUAL(pd.name(), "fcpp");
	BOOST_CHECK_EQUAL(pd.lang(), mpg::output_lang::cpp);
}
BOOST_AUTO_TEST_SUITE_END() // module_part
