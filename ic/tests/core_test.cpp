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
#include "ic/abstract_part.hpp"
#include "ic/algos/split.hpp"

using namespace std::literals;

using mic_outputs_t = std::vector<std::shared_ptr<mic::output>>;

BOOST_AUTO_TEST_SUITE(input_configurator)

struct core_fixture {
	mic::core core;

	std::shared_ptr<mic::input> in = std::make_shared<mic::input>();
	std::shared_ptr<icmocks::output> out = std::make_shared<icmocks::output>();
	std::shared_ptr<icmocks::configuration> config = std::make_shared<icmocks::configuration>();

	std::vector<std::shared_ptr<icmocks::input_node>> nodes;

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

	std::shared_ptr<icmocks::input_node> create_node(std::uint64_t level, std::uint64_t ver)
	{
		auto ret = std::make_shared<icmocks::input_node>();
		MOCK_EXPECT(ret->level).returns(level);
		MOCK_EXPECT(ret->version).returns(ver);
		nodes.emplace_back(ret);
		return ret;
	}

	void create_nodes(std::uint64_t level, std::uint64_t ver, mic::input_node* par, std::size_t cnt)
	{
		std::vector<std::shared_ptr<mic::input_node>> list;
		while(cnt--) list.emplace_back(create_node(level, ver));
		if(par) in->add(par, list);
		else in->add(list);
	}

	void check_input()
	{
		//BOOST_TEST(in->check_tree_levels() == ""s);
	}
};

BOOST_AUTO_TEST_SUITE(core)

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

BOOST_AUTO_TEST_SUITE(input)
BOOST_FIXTURE_TEST_CASE(all, core_fixture)
{
	mic::input im;
	auto n1 = create_node(0,0);
	auto n2 = create_node(0,0);
	im.add({n1, n2});
	BOOST_REQUIRE(im.all().size()==2);
	BOOST_TEST(im.all()[0]==n1);
	BOOST_TEST(im.all()[1]==n2);
	BOOST_CHECK_THROW(im.add({n1}), std::exception);
}
BOOST_FIXTURE_TEST_CASE(children, core_fixture)
{
	mic::input im;
	auto n1 = create_node(0,0);
	auto n2 = create_node(1,0);
	auto n3 = create_node(1,0);
	BOOST_CHECK_THROW(im.add(n1.get(), {n2}), std::exception);
	im.add({n1});
	im.add(n1.get(), {n2});
	BOOST_REQUIRE(im.all().size()==2);
	BOOST_TEST(im.children(n2.get()).size()==0);
	BOOST_REQUIRE(im.children(n1.get()).size()==1);
	BOOST_TEST(im.children(n1.get())[0]==n2.get());
	BOOST_REQUIRE(im.children(nullptr).size()==1);
	BOOST_TEST(im.children(nullptr)[0]==n1.get());
	im.add(n1.get(), {n3});
	BOOST_REQUIRE(im.children(n1.get()).size()==2);
	BOOST_TEST(im.children(n1.get())[1]==n3.get());
	//BOOST_TEST(im.check_tree_levels() == ""s);
}
BOOST_FIXTURE_TEST_CASE(cannot_add_nullptr, core_fixture)
{
	mic::input im;
	auto n1 = create_node(0,0);
	auto n2 = create_node(1,0);
	BOOST_CHECK_THROW(im.add({nullptr}), std::exception);
	im.add({n1});
	BOOST_CHECK_THROW(im.add(n1.get(), {n2,nullptr}), std::exception);
}
BOOST_FIXTURE_TEST_CASE(cannot_add_wrong_level, core_fixture)
{
	mic::input im;

	auto good_root = create_node(0, 0);
	auto bad_root = create_node(1, 0);
	auto bad_child = create_node(0, 0);

	im.add({good_root});
	BOOST_CHECK_THROW(im.add({bad_root}), std::exception);
	BOOST_CHECK_THROW(im.add(good_root.get(), {bad_child}), std::exception);
}
BOOST_AUTO_TEST_SUITE_END() // input

BOOST_AUTO_TEST_SUITE(part)
BOOST_FIXTURE_TEST_CASE(getters, core_fixture)
{
	BOOST_CHECK_THROW(mic::abstract::part(10, "test"s, nullptr), std::exception);
	mic::abstract::part part(10, "test"s, in);
	BOOST_TEST(part.id() == 10);
	BOOST_TEST(part.name() == "test"s);
	BOOST_TEST(part.outputs().empty());
}
BOOST_FIXTURE_TEST_CASE(naming, core_fixture)
{
	auto n1 = std::make_shared<icmocks::input_node>();
	auto n2 = std::make_shared<icmocks::input_node>();
	in->add({n1, n2});
	mic::abstract::part part(10, "test"s, in);
	MOCK_EXPECT(n1->rename).once();
	MOCK_EXPECT(n2->rename).once();
	part.rename(gen_utils::name_conversion::camel_case);
}
BOOST_AUTO_TEST_SUITE(split_versions)
BOOST_FIXTURE_TEST_CASE(simple, core_fixture)
{
	create_nodes(0, 0, nullptr, 2);
	create_nodes(1, 1, nodes[0].get(), 1);
	create_nodes(1, 0, nodes[1].get(), 1);
	create_nodes(1, 1, nodes[1].get(), 1);
	check_input();
	mic::abstract::part part(10, "test"s, in);
	part.split_versions();
	check_input();
	BOOST_TEST(in->children(nullptr).size()==3);
}
BOOST_AUTO_TEST_SUITE(split)
BOOST_FIXTURE_TEST_CASE(roots, core_fixture)
{
	mic::algos::split split;
	BOOST_REQUIRE(split.roots() != nullptr);
	BOOST_TEST(split.roots()->all().empty());
}
BOOST_FIXTURE_TEST_CASE(add_one, core_fixture)
{
	mic::algos::split split;

	create_nodes(0, 0, nullptr, 2);
	create_nodes(1, 0, nodes[0].get(), 1);
	create_nodes(2, 0, nodes[2].get(), 1);
	check_input();

	split(nodes[0]);
	BOOST_TEST(split.roots()->all().size() == 1);
	split(nodes[1]);
	BOOST_TEST(split.roots()->all().size() == 2);
	split(nodes[2]);
	BOOST_TEST(split.roots()->all().size() == 3);
	BOOST_TEST(split.roots()->children(nullptr).size()==2);
	BOOST_TEST(split.roots()->children(nullptr)[0] == nodes[0].get());
	BOOST_TEST(split.roots()->children(nullptr)[1] == nodes[1].get());

	BOOST_TEST(split.roots()->children(nodes[0].get()).size() == 1);
	BOOST_TEST(split.roots()->children(nodes[1].get()).size() == 1);
	split(nodes[3]);
	BOOST_TEST(split.roots()->all().size() == 2);
}
BOOST_AUTO_TEST_SUITE_END() // split
BOOST_AUTO_TEST_SUITE_END() // split_versions
BOOST_AUTO_TEST_SUITE_END() // part

BOOST_AUTO_TEST_SUITE_END() // input_configurator
