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
#include "pg/parts/modules.hpp"
#include "pg/exceptions.hpp"

namespace mpg = modegen::pg;
namespace mpp = modegen::pg::parts;
using namespace std::literals;

auto prov_setts()
{
	auto prov = std::make_shared<pgmocks::mock_provider>();
	auto setts = std::make_shared<mpg::options::container>();
	setts->raw().put("part.fcpp.lang", "cpp");
	return std::make_tuple(prov,setts);
}

BOOST_AUTO_TEST_SUITE(module_part)
BOOST_AUTO_TEST_CASE(constants)
{
	auto [prov,setts] = prov_setts();
	mpp::module_part pd(prov, mpg::options::part_view(setts, "fcpp"sv));
	BOOST_CHECK_EQUAL( pd.name(), "fcpp" );
	BOOST_CHECK_EQUAL( pd.lang(), mpg::output_lang::cpp );
}
BOOST_AUTO_TEST_CASE(outputs_without_build)
{
	auto [prov,setts] = prov_setts();
	mpp::module_part pd(prov, mpg::options::part_view(setts, "fcpp"sv));
	BOOST_CHECK_THROW( pd.outputs(), mpg::errors::error );
}
BOOST_AUTO_TEST_SUITE_END() // module_part
