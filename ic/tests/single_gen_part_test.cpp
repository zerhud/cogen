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
#include "single_gen_part.hpp"

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
	std::shared_ptr<gen_utils_mocks::dsl_manager> t1_dsl = std::make_shared<gen_utils_mocks::dsl_manager>();
	std::shared_ptr<gen_utils_mocks::dsl_manager> t2_dsl = std::make_shared<gen_utils_mocks::dsl_manager>();
	std::shared_ptr<gen_utils_mocks::dsl_manager> t3_dsl = std::make_shared<gen_utils_mocks::dsl_manager>();
	std::shared_ptr<gen_utils_mocks::data_node> t1_root_node = gen_utils_mocks::make_node(0);
	std::shared_ptr<gen_utils_mocks::data_node> t2_root_node = gen_utils_mocks::make_node(0);
	std::shared_ptr<gen_utils_mocks::data_node> t3_root_node = gen_utils_mocks::make_node(0);
	gen_utils::tree t1, t2, t3;

	std::unique_ptr<gen_utils_mocks::compilation_config> compile_cfg
		= std::make_unique<gen_utils_mocks::compilation_config>();
	std::shared_ptr<icmocks::provider> prov = std::make_shared<icmocks::provider>();

	modegen::ic::input all_data;

	single_gen_part_fixture()
	    : t1(t1_root_node, t1_dsl)
	    , t2(t2_root_node, t2_dsl)
	    , t3(t3_root_node, t3_dsl)
	{
		MOCK_EXPECT(t1_dsl->id).returns("t1_dsl");
		MOCK_EXPECT(t2_dsl->id).returns("t2_dsl");
		MOCK_EXPECT(t3_dsl->id).returns("t3_dsl");
	}
};

BOOST_AUTO_TEST_SUITE(input_configurator)
BOOST_AUTO_TEST_SUITE(single_gen_part)

using gen_utils_mocks::make_node;

using modegen::ic::gen_context;
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
	MOCK_EXPECT(t1_dsl->to_json)
	        .calls([this](auto& cfg, const gen_utils::tree& src){
		BOOST_TEST(&src != &t1);
		BOOST_TEST(&cfg == compile_cfg.get());
		boost::json::object ret;
		return ret["a"] = src.children(src.root()).at(0)->node_var()->value, ret;
	});
	MOCK_EXPECT(prov->generate)
	        .once().with("t", R"([{"a":"v1","includes":[]}])"_bj, "v1.cpp");
	MOCK_EXPECT(prov->generate)
	        .once().with("t", R"([{"a":"v2","includes":[]}])"_bj, "v2.cpp");
	sg(gen_context{"${n}.cpp"_s, "t"_s, compile_cfg.get()}, all_data);
}
BOOST_FIXTURE_TEST_CASE(matched_includes, single_gen_part_fixture)
{
	single_gen_part sg(prov.get());
	t1.add(t1.root(), make_node(1, "n", "v1"));
	t1.add(t1.root(), make_node(1, "n", "v2"));
	all_data.add(t1);
	MOCK_EXPECT(t1_dsl->to_json)
	        .calls([this](auto&, const gen_utils::tree&){
		return boost::json::object{};
	});
	MOCK_EXPECT(prov->generate)
	        .once().with("t", R"([{"includes":[]}])"_bj, "v1");
	MOCK_EXPECT(prov->generate)
	        .once().with("t", R"([{"includes":[]}])"_bj, "v2");
	gen_context ctx{"${n}"_s, "t"_s, compile_cfg.get()};
	ctx.generated["a"] = sg(ctx, all_data);

	ctx.map_tmpl = "f.cpp";
	MOCK_EXPECT(prov->generate)
	        .once().with("t", R"([{"includes":[]}])"_bj, "f.cpp");
	sg(ctx, all_data);

	ctx.links.emplace_back("a");
	MOCK_EXPECT(prov->generate)
	        .once().with("t", R"([{"includes":["v1","v2"]}])"_bj, "f.cpp");
	ctx.generated["b"] = sg(ctx, all_data);

	ctx.map_tmpl = "${n}";
	MOCK_EXPECT(prov->generate)
	        .once().with("t", R"([{"includes":["v1"]}])"_bj, "v1");
	MOCK_EXPECT(prov->generate)
	        .once().with("t", R"([{"includes":["v2"]}])"_bj, "v2");
	ctx.generated["c"] = sg(ctx, all_data);
}
BOOST_AUTO_TEST_SUITE_END() // single_gen_part
BOOST_AUTO_TEST_SUITE_END() // input_configurator
