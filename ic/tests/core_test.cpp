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

#include "mocks.hp"
#include "ic/input.hpp"

using namespace std::literals;
using ic_input = modegen::ic::input;

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
		std::function<bool(const ic_input&)> checker;
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
				/*
				MOCK_EXPECT(prov->to_json)
				    .once() .in(json_seq)
				    .with(mic::json_generator::cpp, mock::call(o.checker))
				    .returns(o.compiled_result);
				*/
				MOCK_EXPECT(prov->generate).once()
				    .with(p.tmpl_file, o.compiled_result, o.file);
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
	auto check_input = [](const ic_input& d){ return d.all().size()==2 ; };

	parts.emplace_back(
	            part_info{"p1"sv, "p1f"_s, "p1t"_s, {
	                          {"p1f"sv, R"({"pj":1})"_json, check_input}
	                      }});
	parts.emplace_back(
	            part_info{"p2"sv, "p2f"_s, "p2t"_s, {
	                          {"p2f"sv, R"({"pj":2})"_json, check_input}
	                      }});
	create_parts();

	ic_input all_dsl;
	mock::sequence gen_seq;
	all_dsl.add(gen_utils::tree{gen_utils_mocks::make_node(1), dmanager});
	all_dsl.add(gen_utils::tree{gen_utils_mocks::make_node(2), dmanager});
	MOCK_EXPECT(config->all_dsl).returns(all_dsl);

	core.gen(*config);
}
BOOST_FIXTURE_TEST_CASE(mapping, core_fixture)
{
	auto check_input = [](const ic_input& d){ return d.all().size() == 1; };
	parts.emplace_back(
	            part_info{"p1"sv, "pf${vn}"_s, "pt"_s, {
	                          {"pfv1"sv, R"({"pj":1})"_json, check_input},
	                          {"pfv2"sv, R"({"pj":2})"_json, check_input}
	                      }});
	create_parts();

	ic_input all_dsl;
	gen_utils::tree dsl{gen_utils_mocks::make_node(1, "vn"_s, "v1"_s), dmanager};
	dsl.add(dsl.root(), gen_utils_mocks::make_node(1, "vn"_s, "v2"_s));
	all_dsl.add(std::move(dsl));
	MOCK_EXPECT(config->all_dsl).returns(all_dsl);

	core.gen(*config);
}
BOOST_AUTO_TEST_SUITE_END() // core

BOOST_AUTO_TEST_SUITE(input)
BOOST_AUTO_TEST_CASE(getters)
{
	ic_input i;
	BOOST_TEST(i.all().size() == 0);
	BOOST_TEST(i.select("test"sv).size()==0);
}
BOOST_AUTO_TEST_CASE(adding)
{
	auto m1 = std::make_shared<gen_utils_mocks::dsl_manager>();
	auto m2 = std::make_shared<gen_utils_mocks::dsl_manager>();
	auto m3 = std::make_shared<gen_utils_mocks::dsl_manager>();
	MOCK_EXPECT(m1->id).returns("t1");
	MOCK_EXPECT(m2->id).returns("t2");
	MOCK_EXPECT(m3->id).returns("t3");
	auto n1 = std::make_shared<gen_utils_mocks::data_node>();
	MOCK_EXPECT(n1->version).returns(10);
	gen_utils::tree t1(n1, m1);
	gen_utils::tree t2(n1, m2);
	gen_utils::tree t3(n1, m3);
	ic_input i;
	i.add(t1);
	BOOST_TEST_REQUIRE(i.all().size()==1);
	BOOST_TEST(i.all()[0]->data_id()=="t1"sv);
	BOOST_TEST(i.select("t1"sv).at(0)->data_id()=="t1"sv);

	i.add(t2);
	BOOST_TEST_REQUIRE(i.all().size()==2);
	BOOST_TEST(i.select("t1"sv).at(0)->data_id()=="t1"sv);
	BOOST_TEST(i.select("t2"sv).at(0)->data_id()=="t2"sv);

	i.add(t3);
	BOOST_TEST_REQUIRE(i.all().size()==3);
	BOOST_TEST(i.select("t1"sv).at(0)->data_id()=="t1"sv);
	BOOST_TEST(i.select("t1"sv).at(1)->data_id()=="t1"sv);
}
BOOST_AUTO_TEST_CASE(adding_other)
{
	auto m1 = std::make_shared<gen_utils_mocks::dsl_manager>();
	auto n1 = std::make_shared<gen_utils_mocks::data_node>();
	MOCK_EXPECT(n1->version).returns(10);
	gen_utils::tree t1(n1, m1);
	gen_utils::tree t2(n1, m1);
	gen_utils::tree t3(n1, m1);

	ic_input i1, i2;
	i1.add(t1);
	i1.add(t2);

	BOOST_TEST(i1.all().size()==2);
	BOOST_TEST(i2.all().size()==0);

	i2.add(std::move(i1));
	BOOST_TEST(i2.all().size()==2);
}
BOOST_AUTO_TEST_SUITE_END() // input

BOOST_AUTO_TEST_SUITE_END() // input_configurator
