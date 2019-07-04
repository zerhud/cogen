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

namespace mpg = modegen::pg;
namespace mpo = modegen::pg::options;

MOCK_BASE_CLASS( mock_provider, mpg::provider)
{
	MOCK_METHOD( create_part, 1 )
};

MOCK_BASE_CLASS( mock_part, mpg::part_descriptor )
{
	MOCK_METHOD( name, 0 )
	MOCK_METHOD( outputs, 0 )
};

BOOST_AUTO_TEST_SUITE(without_data)
BOOST_AUTO_TEST_CASE(without_build)
{
	mpg::generator g(nullptr,nullptr);
	BOOST_CHECK_EQUAL(g.parts().count(), 0);
}

BOOST_AUTO_TEST_CASE(withou_generation)
{
	auto setts = std::make_shared<mpo::container>();
	setts->raw().put("part.fcpp.file_single","test.cpp");
	auto prov = std::make_shared<mock_provider>();
	MOCK_EXPECT(prov->create_part).once().returns(nullptr);
	mpg::generator gen(prov,setts);
	BOOST_CHECK_EQUAL(gen.parts().count(), 0);
	gen.build_env();
	BOOST_CHECK_EQUAL(gen.parts().count(), 1);
}
BOOST_AUTO_TEST_SUITE_END() // without_data

BOOST_AUTO_TEST_SUITE(all_to_one)
BOOST_AUTO_TEST_CASE(one_module)
{
	BOOST_FAIL("empty test");
}
BOOST_AUTO_TEST_SUITE_END() // all_to_one
