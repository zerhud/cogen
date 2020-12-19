/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ic_single_gen_part

#include <boost/test/included/unit_test.hpp>
#include <common_utils/tests/mocks.hpp>

#include "mocks.hpp"
#include "generation_part.hpp"

using namespace std::literals;

struct single_gen_part_fixture {
	std::shared_ptr<gen_utils_mocks::dsl_manager> tdsl = std::make_shared<gen_utils_mocks::dsl_manager>();
	std::shared_ptr<gen_utils_mocks::data_node> main_node = gen_utils_mocks::make_node(0);
	gen_utils::tree t1;

	std::shared_ptr<icmocks::provider> prov = std::make_shared<icmocks::provider>();

	modegen::ic::input all_data;

	single_gen_part_fixture()
		: t1(main_node, tdsl)
	{
		MOCK_EXPECT(tdsl->id).returns("test_dsl");
	}
};

BOOST_AUTO_TEST_SUITE(ic)

using gen_utils_mocks::make_node;

using modegen::ic::gen_settings;
using modegen::ic::single_gen_part;

BOOST_FIXTURE_TEST_CASE(main_rules, single_gen_part_fixture)
{
	single_gen_part sg(prov);
	t1.add(t1.root(), make_node(1, "n", "v1"));
	t1.add(t1.root(), make_node(1, "n", "v2"));
	all_data.add(t1);
	MOCK_EXPECT(prov->generate).once().with("tmpl", mock::any, "file_v1.cpp");
	MOCK_EXPECT(prov->generate).once().with("tmpl", mock::any, "file_v2.cpp");
	sg(gen_settings{"file_${n}.cpp"sv, "cpp"sv, "tmpl"sv}, all_data);
}
BOOST_AUTO_TEST_SUITE_END() // ic

