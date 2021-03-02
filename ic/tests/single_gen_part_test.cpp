/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ic_single_gen_part

#include <iomanip>
#include <boost/test/included/unit_test.hpp>
#include <common_utils/tests/mocks.hpp>

#include "mocks.hpp"
#include "single_gen_part.hpp"

using namespace std::literals;
using gunc = gen_utils::name_conversion;
namespace utf = boost::unit_test;

boost::json::value operator "" _bj(const char* d, std::size_t l)
{
	boost::json::parse_options opts{.allow_trailing_commas=true};
	return boost::json::parse(
	            boost::json::string_view(d,l),
	            boost::json::storage_ptr(),
	            opts);
}

struct single_gen_part_fixture {
	std::shared_ptr<gen_utils_mocks::dsl_manager> t1_dsl = std::make_shared<gen_utils_mocks::dsl_manager>();
	std::shared_ptr<gen_utils_mocks::dsl_manager> t2_dsl = std::make_shared<gen_utils_mocks::dsl_manager>();
	std::shared_ptr<gen_utils_mocks::dsl_manager> t3_dsl = std::make_shared<gen_utils_mocks::dsl_manager>();
	std::shared_ptr<gen_utils_mocks::data_node> t1_root_node = gen_utils_mocks::make_node(0);
	std::shared_ptr<gen_utils_mocks::data_node> t2_root_node = gen_utils_mocks::make_node(0);
	std::shared_ptr<gen_utils_mocks::data_node> t3_root_node = gen_utils_mocks::make_node(0);
	gen_utils::tree t1, t2, t3;

	std::unique_ptr<gen_utils::compilation_config> compile_cfg
	    = std::make_unique<gen_utils::compilation_config>();
	std::shared_ptr<icmocks::provider> prov = std::make_shared<icmocks::provider>();

	gen_utils::input all_data;

	single_gen_part_fixture()
	    : t1(t1_root_node, t1_dsl)
	    , t2(t2_root_node, t2_dsl)
	    , t3(t3_root_node, t3_dsl)
	{
		MOCK_EXPECT(t1_dsl->id).returns("t1_dsl");
		MOCK_EXPECT(t2_dsl->id).returns("t2_dsl");
		MOCK_EXPECT(t3_dsl->id).returns("t3_dsl");
	}

	void expect_empty_result(gen_utils_mocks::dsl_manager& dm)
	{
		MOCK_EXPECT(dm.to_json).calls([](auto& ctx, const gen_utils::tree&){
			BOOST_TEST(ctx.links != nullptr);
			return boost::json::object{}; });
	}

	template<typename Arg, typename... Args>
	void expect_empty_result(Arg arg, Args... args)
	{
		expect_empty_result(arg);
		if constexpr (0 < sizeof...(Args))
		        expect_empty_result(args...);
	}

	boost::json::value make_result_json(
	        std::vector<std::string> mincs,
	        std::map<std::string,std::vector<std::string>> rincs,
	        std::optional<boost::json::value> data_val = std::nullopt)
	{
		if(!data_val) data_val = "{\"t1_dsl\":{ }}"_bj;
		std::stringstream json;
		json << "{\"includes\":{";
		if(!mincs.empty()) json << "\"self\":[";
		for(auto& i:mincs) json << "{\"sys\":false,\"file\":\"" << i << "\"},";
		if(!mincs.empty()) json << "] ";
		if(!mincs.empty() && !rincs.empty()) json << ',';
		for(auto& [cond, incs]:rincs) {
			json << std::quoted(cond) << ":[";
			for(auto& i:incs) json
			        << "{\"file\":" << std::quoted(i)
			        << ",\"sys\":false},";
			json << "],";
		}
		json << "}, \"data\":" << *data_val << " }";

		boost::json::parse_options opts{.allow_trailing_commas=true};
		auto result = boost::json::parse(json.str(), boost::json::storage_ptr(), opts);
		return result;
	}
};

BOOST_AUTO_TEST_SUITE(input_configurator)
BOOST_AUTO_TEST_SUITE(single_gen_part)

using gen_utils_mocks::make_node;

using mic::gen_context;
using mic::single_gen_part;

BOOST_AUTO_TEST_CASE(no_provider)
{
	BOOST_CHECK_THROW(single_gen_part(nullptr), std::exception);
}

auto make_adata(std::string t1, std::string t2)
{
	boost::json::object ret;
	ret["t1_dsl"].emplace_object()["a"] = t1;
	ret["t2_dsl"].emplace_object()["a"] = t2;
	return ret;
}

BOOST_FIXTURE_TEST_CASE(main_rules, single_gen_part_fixture)
{
	single_gen_part sg(prov.get());
	t1.add(t1.root(), make_node(1, "n", "n1"));
	t1.add(t1.root(), make_node(1, "n", "n2"));
	t2.add(t2.root(), make_node(2, "m", "m1"));
	t2.add(t2.root(), make_node(2, "m", "m2"));
	all_data.add(t1);
	auto jsoner = [this](auto& ctx, const gen_utils::tree& src){
		BOOST_TEST(&src != &t1);
		BOOST_CHECK(ctx.cfg.naming.size() == 1);
		BOOST_CHECK(ctx.cfg.naming.at(0) == gunc::camel_case);
		boost::json::object ret;
		return ret["a"] = src.children(src.root()).at(0)->node_var()->value, ret;
	};
	MOCK_EXPECT(t1_dsl->to_json).calls(jsoner);
	MOCK_EXPECT(t2_dsl->to_json).calls(jsoner);
	auto data_v1 = make_result_json( {}, {}, R"({"t1_dsl":{"a":"n1"}})"_bj);
	auto data_v2 = make_result_json( {}, {}, R"({"t1_dsl":{"a":"n2"}})"_bj);
	MOCK_EXPECT(prov->generate).once().with("t", data_v1, "n1.cpp");
	MOCK_EXPECT(prov->generate).once().with("t", data_v2, "n2.cpp");
	compile_cfg->naming = {gen_utils::name_conversion::camel_case};
	sg(gen_context{{"${n}.cpp"_s, "t"_s, {}, false, *compile_cfg}, {}}, all_data);

	all_data.add(t2);
	data_v1 = make_result_json( {"n2.cpp"}, {}, make_adata("n1", "m1"));
	data_v2 = make_result_json( {"n1.cpp"}, {}, make_adata("n2", "m1"));
	MOCK_EXPECT(prov->generate).once().with("t", data_v1, "n1.cpp");
	MOCK_EXPECT(prov->generate).once().with("t", data_v2, "n2.cpp");
	sg(gen_context{{"${n}.cpp"_s, "t"_s, {}, false, *compile_cfg}, {}}, all_data);
}
BOOST_FIXTURE_TEST_CASE(main_rules_map_to_both, single_gen_part_fixture)
{
	single_gen_part sg(prov.get());
	t1.add(t1.root(), make_node(1, "n", "n1"));
	t1.add(t1.root(), make_node(1, "n", "n2"));
	t2.add(t2.root(), make_node(2, "m", "m1"));
	t2.add(t2.root(), make_node(2, "m", "m2"));
	all_data.add(t1).add(t2);
	auto jsoner = [this](auto& ctx, const gen_utils::tree& src){
		BOOST_TEST(&src != &t1);
		BOOST_CHECK(ctx.cfg.naming.size() == 1);
		BOOST_CHECK(ctx.cfg.naming.at(0) == gunc::camel_case);
		boost::json::object ret;
		return ret["a"] = src.children(src.root()).at(0)->node_var()->value, ret;
	};
	MOCK_EXPECT(t1_dsl->to_json).calls(jsoner);
	MOCK_EXPECT(t2_dsl->to_json).calls(jsoner);
	auto data_v1 = make_result_json({"n2_m1.cpp"}, {}, make_adata("n1", "m1"));
	auto data_v2 = make_result_json({"n2_m2.cpp"}, {}, make_adata("n1", "m2"));
	auto data_v3 = make_result_json({"n1_m1.cpp"}, {}, make_adata("n2", "m1"));
	auto data_v4 = make_result_json({"n1_m2.cpp"}, {}, make_adata("n2", "m2"));
	MOCK_EXPECT(prov->generate).once().with("t", data_v1, "n1_m1.cpp");
	MOCK_EXPECT(prov->generate).once().with("t", data_v2, "n1_m2.cpp");
	MOCK_EXPECT(prov->generate).once().with("t", data_v3, "n2_m1.cpp");
	MOCK_EXPECT(prov->generate).once().with("t", data_v4, "n2_m2.cpp");
	compile_cfg->naming = {gen_utils::name_conversion::camel_case};
	sg(gen_context{{"${n}_${m}.cpp"_s, "t"_s, {}, false, *compile_cfg}, {}}, all_data);
}
BOOST_FIXTURE_TEST_CASE(split_by_version, single_gen_part_fixture)
{
	single_gen_part sg(prov.get());
	t1.add(t1.root(), make_node(1));
	t1.add(t1.root(), make_node(2));
	all_data.add(t1);

	std::size_t cnt=0;
	MOCK_EXPECT(t1_dsl->to_json).calls([&cnt](auto& ctx, const gen_utils::tree& t){
		BOOST_TEST(ctx.links != nullptr);
		return boost::json::value{std::to_string(++cnt)}; });

	auto empty_result = make_result_json({}, {},
			R"({"t1_dsl":["1"]})"_bj);
	MOCK_EXPECT(prov->generate).once().with("t", empty_result, "f.cpp");

	sg(gen_context{{"f.cpp"_s, "t"_s, {}, true, *compile_cfg}, {}}, all_data);
}
BOOST_FIXTURE_TEST_CASE(matched_includes, single_gen_part_fixture)
{
	single_gen_part sg(prov.get());
	t1.add(t1.root(), make_node(1, "n", "v1"));
	t1.add(t1.root(), make_node(1, "n", "v2"));
	all_data.add(t1);
	expect_empty_result(*t1_dsl);
	auto empty_data = make_result_json({},{});
	MOCK_EXPECT(prov->generate).once().with("t", empty_data, "v1");
	MOCK_EXPECT(prov->generate).once().with("t", empty_data, "v2");
	gen_context ctx{{"${n}"_s, "t"_s, {}, false, *compile_cfg.get()}, {}};

	ctx.generated["a"] = sg(ctx, all_data);

	boost::json::value mincs_data = make_result_json({"v1", "v2"}, {});

	ctx.cfg_part.map_tmpl = "f.cpp";
	MOCK_EXPECT(prov->generate).once().with("t", mincs_data, "f.cpp");
	sg(ctx, all_data);

	ctx.cfg_part.links.emplace_back("a");
	MOCK_EXPECT(prov->generate).once().with("t", mincs_data, "f.cpp");
	ctx.generated["b"] = sg(ctx, all_data);

	mincs_data = make_result_json({"v1"}, {});
	ctx.cfg_part.map_tmpl = "_${n}";
	MOCK_EXPECT(prov->generate).once().with("t", mincs_data, "_v1");
	mincs_data = make_result_json({"v2"}, {});
	MOCK_EXPECT(prov->generate).once().with("t", mincs_data, "_v2");
	ctx.generated["c"] = sg(ctx, all_data);
}
BOOST_FIXTURE_TEST_CASE(required_includes, single_gen_part_fixture)
{
	single_gen_part sg(prov.get());
	auto t1_child1 = make_node(1, "n", "v1", "t1_a");
	auto t1_child2 = make_node(1, "n", "v2", "t1_b");
	auto t1_child3 = make_node(1, "n", "v2", "t1_c");
	MOCK_EXPECT(t1_child1->link_condition).returns("cond1");
	MOCK_EXPECT(t1_child2->link_condition).returns("cond1");
	MOCK_EXPECT(t1_child3->link_condition).returns("cond1");
	MOCK_EXPECT(t1_child1->imports_modification).returns(std::nullopt);
	MOCK_EXPECT(t1_child2->imports_modification)
	        .returns(gen_utils::import_file{false, "vector"});
	MOCK_EXPECT(t1_child3->imports_modification)
	        .returns(gen_utils::import_file{false, "vector"});
	t1.add(t1.root(), t1_child1);
	t1.add(t1.root(), t1_child2);
	t1.add(t1.root(), t1_child3);
	all_data.add(t1);
	expect_empty_result(*t1_dsl, *t2_dsl);

	auto empty_data = make_result_json({}, {});
	MOCK_EXPECT(prov->generate).once().with("t", empty_data, "v1");
	MOCK_EXPECT(prov->generate).once().with("t", empty_data, "v2");
	compile_cfg->naming.emplace_back(gunc::camel_case);
	gen_context ctx{{"${n}"_s, "t"_s, {}, false, *compile_cfg.get()}, {}};
	ctx.generated["part1"] = sg(ctx, all_data);
	BOOST_TEST(ctx.generated["part1"]["v1"].conf().naming.size() == 2);
	BOOST_CHECK(ctx.generated["part1"]["v1"].conf().naming.at(1) == gunc::camel_case);

	t2.add( t2.root(), make_node(
	            2, std::nullopt, std::nullopt,
	            "t2_a", {{"t1_a"}, {"t1_b"}, {"t1_c"}}));
	gen_utils::input other_data;
	other_data.add(t2);
	ctx.cfg_part.map_tmpl = "file";
	ctx.cfg_part.links.emplace_back("part1");
	MOCK_EXPECT(prov->generate).once()
	        .with("t", make_result_json(
	            {},
	            {{"cond1", {"v1", "vector"}}},
	            "{\"t2_dsl\":{}}"_bj),
	        "file");
	ctx.generated["part2"] = sg(ctx, other_data);
}
BOOST_FIXTURE_TEST_CASE(crossed_includes, single_gen_part_fixture, *utf::disabled())
{
	t1.add(t1.root(), make_node(100, "v1", "n1"));
	t1.add(t1.root(), make_node(110, "v1", "n2"));
	t2.add(t2.root(), make_node(210, "v2", "m1"));
	gen_utils::input all_data;
	all_data.add(t1).add(t2);

	single_gen_part part(prov.get());
	gen_context ctx{{"${v1}"_s, "t"_s, {}, false, *compile_cfg.get()}, {}};

	expect_empty_result(*t1_dsl, *t2_dsl);
	MOCK_EXPECT(prov->generate).exactly(2);
	ctx.generated["p1"] = part(ctx, all_data);

	ctx.cfg_part.links.emplace_back("p1");
	MOCK_EXPECT(prov->generate).once()
	        .with("t", make_result_json(
	            {},
	            {{"t1_dsl", {"v1", "vector"}}},
	            "{\"t2_dsl\":{}}"_bj),
	        "file");
	ctx.generated["p2"] = part(ctx, all_data);
}
BOOST_AUTO_TEST_SUITE_END() // single_gen_part
BOOST_AUTO_TEST_SUITE_END() // input_configurator
