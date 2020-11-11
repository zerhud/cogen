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

#include <nlohman/json.hpp>

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

BOOST_AUTO_TEST_SUITE(input_configurator)

struct core_fixture {
	std::shared_ptr<icmocks::configuration> config
		= std::make_shared<icmocks::configuration>();
	std::shared_ptr<icmocks::provider> prov
		= std::make_shared<icmocks::provider>();
	mic::core core;

	core_fixture() : core(prov)
	{
	}

	void set_config(std::string_view id, std::pmr::string tmpl, std::pmr::string mt)
	{
		MOCK_EXPECT(config->map_tmpl).with(id).returns(mt);
		MOCK_EXPECT(config->tmpl_file).with(id).returns(tmpl);
		MOCK_EXPECT(config->lang).with(id).returns(mic::json_generator::cpp);
	}

	void expect_mods(std::string_view id, bool sv, gen_utils::name_conversion n)
	{
		MOCK_EXPECT(config->split_versions).with(id).returns(sv);
		MOCK_EXPECT(config->naming).with(id).returns(n);
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
	auto p1j = R"({"pj":1})"_json;
	auto p2j = R"({"pj":2})"_json;
	MOCK_EXPECT(config->parts).returns(make_vector("p1"sv, "p2"sv));
	MOCK_EXPECT(prov->generate).once().with("p1t", p1j, "p1f"sv);
	MOCK_EXPECT(prov->generate).once().with("p2t", p2j, "p2f"sv);
	set_config("p1"sv, "p1t"_s, "p1f"_s);
	set_config("p2"sv, "p2t"_s, "p2f"_s);

	ic_input all_dsl;
	mock::sequence gen_seq;
	auto check_input = [](const ic_input& d){ return d.all().size()==2 ; };
	all_dsl.add(gen_utils::tree{gen_utils_mocks::make_node(1), "p1"_s});
	all_dsl.add(gen_utils::tree{gen_utils_mocks::make_node(2), "p2"_s});
	MOCK_EXPECT(config->all_dsl).returns(all_dsl);
	MOCK_EXPECT(prov->to_json)
	        .once() .in(gen_seq)
	        .with(json_generator::cpp, mock::call(check_input))
	        .returns(p1j);
	MOCK_EXPECT(prov->to_json)
	        .once() .in(gen_seq)
	        .with(json_generator::cpp, mock::call(check_input))
	        .returns(p2j);

	core.gen(*config);
}
BOOST_FIXTURE_TEST_CASE(mapping, core_fixture)
{
	auto pj1 = R"({"pj":1})"_json;
	auto pj2 = R"({"pj":2})"_json;
	MOCK_EXPECT(config->parts).returns(make_vector("p1"sv));
	MOCK_EXPECT(prov->generate).once().with("pt", pj1, "pfv1"sv);
	MOCK_EXPECT(prov->generate).once().with("pt", pj2, "pfv2"sv);
	set_config("p1"sv, "pt"_s, "pf${var}"_s);

	ic_input all_dsl;
	gen_utils::tree dsl_tree{gen_utils_mocks::make_node(1, "var"_s, "v1"_s), "p1"_s};
	dsl_tree.add(dsl_tree.root(), gen_utils_mocks::make_node(1, "var"_s, "v2"_s));
	all_dsl.add(std::move(dsl_tree));
	MOCK_EXPECT(config->all_dsl).returns(all_dsl);

	mock::sequence gen_seq;
	MOCK_EXPECT(prov->to_json).once().in(gen_seq).returns(pj1);
	MOCK_EXPECT(prov->to_json).once().in(gen_seq).returns(pj2);

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
	auto n1 = std::make_shared<gen_utils_mocks::data_node>();
	MOCK_EXPECT(n1->version).returns(10);
	gen_utils::tree t1(n1, "t1");
	gen_utils::tree t2(n1, "t2");
	gen_utils::tree t3(n1, "t1");
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
	auto n1 = std::make_shared<gen_utils_mocks::data_node>();
	MOCK_EXPECT(n1->version).returns(10);
	gen_utils::tree t1(n1, "t1");
	gen_utils::tree t2(n1, "t2");
	gen_utils::tree t3(n1, "t1");

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
