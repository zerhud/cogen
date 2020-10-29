/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ic_abstract_part

#include <boost/test/included/unit_test.hpp>

#include "abstract_part.hpp"
#include "common_utils/tests/mocks.hpp"

using namespace std::literals;

namespace mic = modegen::ic;

BOOST_AUTO_TEST_SUITE(input_configurator)
BOOST_AUTO_TEST_SUITE(abstract_part)
using abs_part = modegen::ic::abstract::part;
BOOST_AUTO_TEST_CASE(getters)
{
	abs_part p(10, "name", mic::input{});
	BOOST_TEST(p.id()==10);
	BOOST_TEST(p.name()=="name"s);
}
BOOST_AUTO_TEST_SUITE(map_to)
struct fixture {
	mic::input data;
	std::optional<abs_part> _part;
	fixture() =default ;

	abs_part& part() {
		if(!_part) _part.emplace(10, "name", data);
		return *_part;
	}

	std::shared_ptr<gen_utils_mocks::data_node> make_node(
			  std::optional<std::uint64_t> v
			, std::optional<std::pmr::string> name=std::nullopt
			, std::optional<std::pmr::string> value=std::nullopt
			) const
	{
		return gen_utils_mocks::make_node(v, std::move(name), std::move(value));
	}
};
BOOST_FIXTURE_TEST_CASE(no_vars, fixture)
{
	auto root = make_node(10);
	data.add(gen_utils::tree(root, "mods"));
	part().map_to("no_vars"s);
	BOOST_TEST(part().compiled_input().size()==1);
	BOOST_TEST(part().compiled_input().at(0).map=="no_vars"sv);
}
BOOST_FIXTURE_TEST_CASE(one_variable, fixture)
{
	auto root = make_node(10);
	gen_utils::tree mt(root, "mods");
	auto mod = make_node(std::nullopt, "mod", "m1");
	mt.add(mt.root(), mod);
	data.add(mt);

	part().map_to("${mod}_v"sv);
	BOOST_TEST(part().compiled_input().size()==1);
	BOOST_TEST(part().compiled_input().at(0).map=="m1_v"sv);
}
BOOST_FIXTURE_TEST_CASE(twice_call, fixture)
{
	auto root = make_node(10);
	data.add(gen_utils::tree(root, "mods"));
	part().map_to("no_vars"s);
	BOOST_CHECK_THROW(part().map_to("no_vars"), std::exception);
}
BOOST_AUTO_TEST_SUITE_END() // map_to
BOOST_AUTO_TEST_SUITE_END() // abstract_part
BOOST_AUTO_TEST_SUITE_END() // input_configurator

