/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE part_descriptors

#include <boost/test/unit_test.hpp>
#include <turtle/mock.hpp>

#include "parser/interface/loader.hpp"

#include "generation/provider.hpp"
#include "generation/file_data.hpp"
#include "generation/interface/part_descriptor.hpp"
#include "errors.h"

using namespace std::literals;
namespace mg = modegen::generation;
namespace mo = modegen::generation::options;
namespace mi = modegen::generation::interface;
namespace pi = modegen::parser::interface;

struct fake_mi_loader : public pi::loader {
	std::vector<pi::module> data;

	void load(std::istream& input, std::string fn) override {}
	void load(FS::path file) override {}
	void finish_loads() override {}

	std::vector<pi::module> result() const override
	{
		return data;
	}
};

BOOST_AUTO_TEST_SUITE(interface)
BOOST_AUTO_TEST_CASE(no_mods)
{
	auto ldr = std::make_shared<fake_mi_loader>();

	mo::container_ptr opt_cnt = std::make_shared<mo::container>();
	opt_cnt->raw().put("gen.def.output", "%mod%.ext");
	mi::part_descriptor pd(mo::view(opt_cnt, "def"sv), {ldr});

	BOOST_CHECK_EQUAL(pd.need_output(), false);
}
BOOST_AUTO_TEST_CASE(one_mod)
{
	auto ldr = std::make_shared<fake_mi_loader>();
	ldr->data = pi::parse("module mod v1.0: int foo();"sv).mods;

	mo::container_ptr opt_cnt = std::make_shared<mo::container>();
	opt_cnt->raw().put("gen.def.output", "%mod%.ext");
	mi::part_descriptor pd(mo::view(opt_cnt, "def"sv), {ldr});

	BOOST_CHECK_EQUAL(pd.part_name(), "def");
	BOOST_CHECK_EQUAL(pd.need_output(), true);
	BOOST_CHECK_EQUAL(pd.file_name(), "mod.ext");
	BOOST_CHECK_EQUAL(pd.next(), false);
}
BOOST_AUTO_TEST_CASE(splited_by_ver_modules)
{
	BOOST_FAIL("no test");
}
BOOST_AUTO_TEST_SUITE(wrong)
BOOST_AUTO_TEST_CASE(wrong_loader)
{
	// test may be only idl or data for now
	// tests: cannot dynamic cast
	BOOST_FAIL("no test");
}
BOOST_AUTO_TEST_SUITE_END() // wrong
BOOST_AUTO_TEST_SUITE_END() // interface
