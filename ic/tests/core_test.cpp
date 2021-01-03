/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ic_core

#include <boost/test/included/unit_test.hpp>
#include <common_utils/tests/mocks.hpp>

#include "mocks.hpp"
#include "input/input.hpp"

using namespace std::literals;
using ic_input = gen_utils::input;

template <class T0, class... Ts>
auto make_vector(T0&& first, Ts&&... args)
{
	using first_type = std::decay_t<T0>;
	return std::pmr::vector<first_type>{
			std::forward<T0>(first),
			std::forward<Ts>(args)...
	};
}

std::pmr::string operator "" _s (const char* d, std::size_t l)
{
	return std::pmr::string(d, l);
}

boost::json::value operator "" _json (const char* d, std::size_t l)
{
	return boost::json::parse(std::string_view(d,l));
}

BOOST_AUTO_TEST_SUITE(input_configurator)

struct core_fixture {
	std::shared_ptr<gen_utils_mocks::dsl_manager> dmanager =
			std::make_shared<gen_utils_mocks::dsl_manager>();
	std::shared_ptr<icmocks::configuration> config
		= std::make_shared<icmocks::configuration>();
	std::shared_ptr<icmocks::provider> prov
		= std::make_shared<icmocks::provider>();
	mic::core core;

	struct output_info {
		std::string_view file;
		boost::json::value compiled_result;
	};

	struct part_info {
		std::string_view name;
		std::pmr::string map_tmpl;
		std::pmr::string tmpl_file;
		std::vector<output_info> outputs;
	};

	std::vector<part_info> parts;

	core_fixture() : core(prov)
	{
		MOCK_EXPECT(dmanager->id).returns("data_id"sv);
	}

	void expect_mods(std::string_view id, bool sv, gen_utils::name_conversion n)
	{
		MOCK_EXPECT(config->split_versions).with(id).returns(sv);
		MOCK_EXPECT(config->naming).with(id).returns(n);
	}

	void create_parts()
	{
		std::pmr::vector<std::string_view> names;
		for(auto& p:parts) {
			names.emplace_back(p.name);
			MOCK_EXPECT(config->map_tmpl)
			        .with(p.name).returns(p.map_tmpl);
			MOCK_EXPECT(config->tmpl_file)
			        .with(p.name).returns(p.tmpl_file);
			MOCK_EXPECT(config->lang)
			        .with(p.name).returns(mic::json_generator::cpp);

			mock::sequence json_seq;
			for(auto& o:p.outputs) {
				MOCK_EXPECT(dmanager->to_json)
				    .once() .in(json_seq)
				    .with(mock::any, mock::any)
				    .returns(o.compiled_result);
				boost::json::array cr;
				cr.emplace_back(o.compiled_result);
				MOCK_EXPECT(prov->generate).once()
				    .with(p.tmpl_file, cr, o.file);
			}
		}
		MOCK_EXPECT(config->parts).returns(names);
	}
};

BOOST_AUTO_TEST_SUITE(core)
using mic::json_generator;
BOOST_AUTO_TEST_CASE(check_json)
{
	BOOST_TEST(R"({"a":1,"b":2})"_json == R"({"b":2,"a":1})"_json);
}
BOOST_FIXTURE_TEST_CASE(generation, core_fixture)
{
	parts.emplace_back(
	            part_info{"p1"sv, "p1f"_s, "p1t"_s, {
	                          {"p1f"sv, R"({"pj":1})"_json}
	                      }});
	parts.emplace_back(
	            part_info{"p2"sv, "p2f"_s, "p2t"_s, {
	                          {"p2f"sv, R"({"pj":2})"_json}
	                      }});
	create_parts();

	ic_input all_dsl;
	gen_utils::tree dsl{gen_utils_mocks::make_node(1), dmanager};
	dsl.add(dsl.root(), gen_utils_mocks::make_node(2));
	all_dsl.add(std::move(dsl));
	MOCK_EXPECT(config->all_dsl).returns(all_dsl);

	core.gen(*config);
}
BOOST_FIXTURE_TEST_CASE(mapping, core_fixture)
{
	parts.emplace_back(
	            part_info{"p1"sv, "pf${vn}"_s, "pt"_s, {
	                          {"pfv1"sv, R"({"pj":1})"_json},
	                          {"pfv2"sv, R"({"pj":2})"_json}
	                      }});
	create_parts();

	ic_input all_dsl;
	gen_utils::tree dsl{gen_utils_mocks::make_node(1), dmanager};
	dsl.add(dsl.root(), gen_utils_mocks::make_node(1, "vn"_s, "v1"_s));
	dsl.add(dsl.root(), gen_utils_mocks::make_node(1, "vn"_s, "v2"_s));
	all_dsl.add(std::move(dsl));
	MOCK_EXPECT(config->all_dsl).returns(all_dsl);

	core.gen(*config);
}
BOOST_AUTO_TEST_SUITE_END() // core

BOOST_AUTO_TEST_SUITE_END() // input_configurator
