/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE part_descriptor

#include <boost/test/unit_test.hpp>

#include "mocks.hpp"
#include "pg/info_part.hpp"
#include "pg/exceptions.hpp"
#include "pg/part_manager.hpp"
#include "parser/interface/grammar.hpp"

namespace mpi = modegen::parser::interface;
namespace mpg = modegen::pg;
using namespace std::literals;

typedef std::tuple<mpg::info_part> part_types;

auto prov_setts()
{
	auto prov = std::make_shared<pgmocks::provider>();
	auto setts = std::make_shared<mpg::options::container>();
	setts->raw().put("part.fcpp.lang", "cpp");
	return std::make_tuple(prov,setts);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(constants, T, part_types)
{
	auto [prov,setts] = prov_setts();
	T pd(mpg::options::part_view(setts, "fcpp"sv));
	BOOST_CHECK_EQUAL( pd.name(), "fcpp" );
	BOOST_CHECK_EQUAL( pd.lang(), mpg::output_lang::cpp );
}

BOOST_AUTO_TEST_CASE(outputs_without_build)
{
	auto [prov,setts] = prov_setts();
	setts->raw().put("part.fcpp.file_single", "test.cpp");
	mpg::info_part pd(mpg::options::part_view(setts, "fcpp"sv));
	BOOST_CHECK_THROW( pd.outputs(), mpg::errors::error );
}
BOOST_AUTO_TEST_SUITE(single_output)
BOOST_AUTO_TEST_CASE(one_module)
{
	auto pf = mpi::parse("module mod1 v1.1:");
	auto ldr = std::make_shared<pgmocks::iloader>();
	auto output = std::make_shared<pgmocks::part_output>();
	auto [prov,setts] = prov_setts();
	setts->raw().put("part.fcpp.file_single", "test.cpp");
	mpg::part_manager pman;
	auto pd = std::make_shared<mpg::info_part>(mpg::options::part_view(setts, "fcpp"sv));

	MOCK_EXPECT(output->override_setts);
	MOCK_EXPECT(ldr->result).returns(pf.mods);
	MOCK_EXPECT(prov->input).returns(std::vector<modegen::parser::loader_ptr>{ldr});
	MOCK_EXPECT(prov->create_output).once().with(mpg::output_lang::cpp, "test.cpp"s).returns(output);
	pd->build_outputs(pman, prov) ;
	auto outs = pd->outputs();

	BOOST_REQUIRE_EQUAL(outs.size(), 1);
}
BOOST_AUTO_TEST_SUITE_END() // single_output
BOOST_AUTO_TEST_SUITE(bymod_output)
BOOST_AUTO_TEST_CASE(normal)
{
	auto pf = mpi::parse("module mod1 v1.1: module mod2 v2.2:");
	auto ldr = std::make_shared<pgmocks::iloader>();
	auto output = std::make_shared<pgmocks::part_output>();
	auto [prov,setts] = prov_setts();
	setts->raw().put("part.fcpp.file_bymod", "test_$mod_$va_$vi.cpp");
	mpg::part_manager pman;
	auto pd = std::make_shared<mpg::info_part>(mpg::options::part_view(setts, "fcpp"sv));

	MOCK_EXPECT(output->override_setts);
	MOCK_EXPECT(ldr->result).returns(pf.mods);
	MOCK_EXPECT(prov->input).returns(std::vector<modegen::parser::loader_ptr>{ldr});
	MOCK_EXPECT(prov->create_output).once().with(mpg::output_lang::cpp, "test_mod1_1_1.cpp"s).returns(output);
	MOCK_EXPECT(prov->create_output).once().with(mpg::output_lang::cpp, "test_mod2_2_2.cpp"s).returns(output);
	pd->build_outputs(pman, prov) ;
	auto outs = pd->outputs();

	BOOST_REQUIRE_EQUAL(outs.size(), 2);
}
BOOST_AUTO_TEST_SUITE_END() // bymod_output
