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
#include "pg/parts/modules.hpp"
#include "pg/exceptions.hpp"
#include "pg/part_manager.hpp"
#include "parser/interface/grammar.hpp"

namespace mpi = modegen::parser::interface;
namespace mpg = modegen::pg;
namespace mpp = modegen::pg::parts;
using namespace std::literals;

typedef std::tuple<mpp::module_part> part_types;

auto prov_setts()
{
	auto prov = std::make_shared<pgmocks::mock_provider>();
	auto setts = std::make_shared<mpg::options::container>();
	setts->raw().put("part.fcpp.lang", "cpp");
	setts->raw().put("part.fcpp.file_single", "test.cpp");
	return std::make_tuple(prov,setts);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(constants, T, part_types)
{
	auto [prov,setts] = prov_setts();
	T pd(prov, mpg::options::part_view(setts, "fcpp"sv));
	BOOST_CHECK_EQUAL( pd.name(), "fcpp" );
	BOOST_CHECK_EQUAL( pd.lang(), mpg::output_lang::cpp );
}

BOOST_AUTO_TEST_SUITE(module_part)
BOOST_AUTO_TEST_CASE(outputs_without_build)
{
	auto [prov,setts] = prov_setts();
	mpp::module_part pd(prov, mpg::options::part_view(setts, "fcpp"sv));
	BOOST_CHECK_THROW( pd.outputs(), mpg::errors::error );
}
BOOST_AUTO_TEST_SUITE(single_output)
BOOST_AUTO_TEST_CASE(one_module)
{
	auto pf = mpi::parse("module mod1 v1.1:");
	auto ldr = std::make_shared<pgmocks::iloader>();
	auto [prov,setts] = prov_setts();
	mpg::part_manager pman;
	auto pd = std::make_shared<mpp::module_part>(prov, mpg::options::part_view(setts, "fcpp"sv));

	MOCK_EXPECT(ldr->result).returns(pf.mods);
	MOCK_EXPECT(prov->input).returns(std::vector<modegen::parser::loader_ptr>{ldr});
	MOCK_EXPECT(prov->create_output).once().with(mpg::output_lang::cpp, "test.cpp"s).returns(nullptr);
	pd->build_outputs(pman, *prov) ;
	auto outs = pd->outputs();

	BOOST_REQUIRE_EQUAL(outs.size(), 1);
}
BOOST_AUTO_TEST_SUITE_END() // single_output
BOOST_AUTO_TEST_SUITE_END() // module_part
