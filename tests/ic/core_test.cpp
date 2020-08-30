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

#include "mocks.hp"

using namespace std::literals;

using mic_outputs_t = std::vector<std::shared_ptr<mic::output>>;

BOOST_AUTO_TEST_SUITE(input_configurator)
BOOST_AUTO_TEST_SUITE(core)

struct core_fixture {
	mic::core core;

	std::shared_ptr<icmocks::input> in = std::make_shared<icmocks::input>();
	std::shared_ptr<icmocks::output> out = std::make_shared<icmocks::output>();
	std::shared_ptr<icmocks::configuration> config = std::make_shared<icmocks::configuration>();

	std::vector<std::shared_ptr<icmocks::generation_part>> parts;
	std::vector<std::shared_ptr<mic::generation_part>> _parts; ///< simplify returns
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

	void set_config(std::size_t id, std::string tmpl, std::string mt)
	{
		MOCK_EXPECT(config->map_tmpl).with(id).returns(mt);
		MOCK_EXPECT(config->tmpl_information).with(id).returns(tmpl);
	}

	void expect_mods(std::size_t id, bool sv, gen_utils::name_conversion n)
	{
		MOCK_EXPECT(config->split_versions).with(id).returns(sv);
		if(sv) MOCK_EXPECT(parts[id]->split_versions).once();
		MOCK_EXPECT(config->naming).with(id).returns(n);
		MOCK_EXPECT(parts[id]->rename).once().with(n);
	}
};

BOOST_FIXTURE_TEST_CASE(errors, core_fixture)
{
	BOOST_CHECK_THROW(core.gen(nullptr, config), std::exception);
	BOOST_CHECK_THROW(core.gen(out, nullptr), std::exception);
}

BOOST_FIXTURE_TEST_CASE(map_to, core_fixture)
{
	create_parts(2);
	assert(parts.size()==2);
	MOCK_EXPECT(config->parts).once().returns(_parts);

	set_config(0, "tmpl1", "p0");
	set_config(1, "tmpl2", "p1");
	MOCK_EXPECT(config->output_dir).returns("/test/dir/"s);
	expect_mods(0, true, gen_utils::name_conversion::as_is);
	expect_mods(1, false, gen_utils::name_conversion::underscore);

	auto out1 = std::make_shared<icmocks::output>();
	auto out2 = std::make_shared<icmocks::output>();

	mock::sequence building1, building2;
	MOCK_EXPECT(parts[0]->map_to).in(building1).once().with("p0"s);
	MOCK_EXPECT(parts[0]->outputs).once().in(building1).returns(mic_outputs_t{out1});
	MOCK_EXPECT(out1->gen).in(building1).once().with("/test/dir/", "tmpl1");

	MOCK_EXPECT(parts[1]->map_to).in(building2).once().with("p1"s);
	MOCK_EXPECT(parts[1]->outputs).once().in(building2).returns(mic_outputs_t{out2});
	MOCK_EXPECT(out2->gen).in(building2).once().with("/test/dir/", "tmpl2");

	core.gen(out, config);
}

BOOST_AUTO_TEST_SUITE_END() // core

BOOST_AUTO_TEST_SUITE(input_data)
BOOST_AUTO_TEST_SUITE_END() // input_data

BOOST_AUTO_TEST_SUITE_END() // input_configurator
