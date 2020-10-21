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
#include "ic/abstract_part.hpp"
#include "ic/input.hpp"

using namespace std::literals;

BOOST_AUTO_TEST_SUITE(input_configurator)

struct core_fixture {
	mic::core core;

	std::shared_ptr<icmocks::configuration> config = std::make_shared<icmocks::configuration>();

	std::pmr::vector<std::shared_ptr<icmocks::generation_part>> parts;
	std::pmr::vector<std::shared_ptr<mic::generation_part>> _parts; ///< simplify returns
	void create_parts(std::size_t cnt)
	{
		while(cnt--) create_part();
	}

	std::shared_ptr<icmocks::generation_part> create_part()
	{
		auto p = std::make_shared<icmocks::generation_part>();
		MOCK_EXPECT(p->id).returns(parts.size());
		_parts.emplace_back(p);
		return parts.emplace_back(p);
	}

	void set_config(std::size_t id, std::pmr::string tmpl, std::pmr::string mt)
	{
		MOCK_EXPECT(config->map_tmpl).with(id).returns(mt);
		MOCK_EXPECT(config->tmpl_file).with(id).returns(tmpl);
	}

	void expect_mods(std::size_t id, bool sv, gen_utils::name_conversion n)
	{
		MOCK_EXPECT(config->split_versions).with(id).returns(sv);
		if(sv) MOCK_EXPECT(parts[id]->split_versions).once();
		MOCK_EXPECT(config->naming).with(id).returns(n);
		MOCK_EXPECT(parts[id]->rename).once().with(n);
	}
};

BOOST_AUTO_TEST_SUITE(core)
BOOST_FIXTURE_TEST_CASE(errors, core_fixture)
{
	BOOST_CHECK_THROW(core.gen(nullptr), std::exception);
}
BOOST_FIXTURE_TEST_CASE(generation, core_fixture)
{
	create_parts(2);
	assert(parts.size()==2);
	MOCK_EXPECT(config->parts).at_least(1).returns(_parts);

	set_config(0, "tmpl1", "p0");
	set_config(1, "tmpl2", "p1");
	expect_mods(0, true, gen_utils::name_conversion::as_is);
	expect_mods(1, false, gen_utils::name_conversion::underscore);

	mock::sequence building;
	MOCK_EXPECT(parts[0]->map_to).in(building).once().with("p0"s);
	MOCK_EXPECT(parts[1]->map_to).in(building).once().with("p1"s);

	using modegen::ic::map_result;
	std::pmr::vector<map_result> part_0_compiled;
	part_0_compiled.emplace_back(map_result{"name", modegen::ic::input()});
	std::pmr::vector<map_result> part_1_compiled;
	part_1_compiled.emplace_back(map_result{"name11", modegen::ic::input()});
	part_1_compiled.emplace_back(map_result{"name12", modegen::ic::input()});
	MOCK_EXPECT(parts[0]->compiled_input).returns(part_0_compiled);
	MOCK_EXPECT(parts[1]->compiled_input).returns(part_1_compiled);

	MOCK_EXPECT(config->generate).once().in(building).calls([](auto f, const auto& d){
		BOOST_TEST(f=="tmpl1"s);
		BOOST_TEST(d.map=="name");
	});
	MOCK_EXPECT(config->generate).once().in(building).calls([](auto f, const auto& d){
		BOOST_TEST(f=="tmpl2"s);
		BOOST_TEST(d.map=="name11");
	});
	MOCK_EXPECT(config->generate).once().in(building).calls([](auto f, const auto& d){
		BOOST_TEST(f=="tmpl2"s);
		BOOST_TEST(d.map=="name12");
	});

	core.gen(config);
}
BOOST_AUTO_TEST_SUITE_END() // core

BOOST_AUTO_TEST_SUITE(input)
using ic_input = modegen::ic::input;
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
	BOOST_FAIL("not ready yet");
}
BOOST_AUTO_TEST_SUITE_END() // input

BOOST_AUTO_TEST_SUITE_END() // input_configurator
