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

std::pmr::string operator "" _s (const char* d, std::size_t l)
{
	return std::pmr::string(d, l);
}

boost::json::value operator "" _bj(const char* d, std::size_t l)
{
	return boost::json::parse(std::string_view(d,l));
}

struct single_gen_part_fixture {
	std::shared_ptr<gen_utils_mocks::dsl_manager> tdsl = std::make_shared<gen_utils_mocks::dsl_manager>();
	std::shared_ptr<gen_utils_mocks::data_node> main_node = gen_utils_mocks::make_node(0);
	gen_utils::tree t1;

	std::unique_ptr<gen_utils_mocks::compilation_config> compile_cfg
		= std::make_unique<gen_utils_mocks::compilation_config>();
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

BOOST_AUTO_TEST_CASE(no_provider)
{
	BOOST_CHECK_THROW(single_gen_part(nullptr), std::exception);
}

BOOST_FIXTURE_TEST_CASE(main_rules, single_gen_part_fixture)
{
	single_gen_part sg(prov.get());
	t1.add(t1.root(), make_node(1, "n", "v1"));
	t1.add(t1.root(), make_node(1, "n", "v2"));
	all_data.add(t1);
	MOCK_EXPECT(tdsl->to_json).calls([this](auto& cfg, const gen_utils::tree& src){
		BOOST_TEST(&src != &t1);
		BOOST_TEST(&cfg == compile_cfg.get());
		boost::json::object ret;
		return ret["a"] = src.children(src.root()).at(0)->node_var()->value, ret;
	});
	MOCK_EXPECT(prov->generate).once().with("t", R"([{"a":"v1"}])"_bj, "v1.cpp");
	MOCK_EXPECT(prov->generate).once().with("t", R"([{"a":"v2"}])"_bj, "v2.cpp");
	sg(gen_settings{"${n}.cpp"_s, "t"_s, compile_cfg.get()}, all_data);
}
BOOST_AUTO_TEST_SUITE_END() // ic

