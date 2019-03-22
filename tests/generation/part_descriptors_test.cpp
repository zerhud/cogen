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
#include "parser/data_tree/loader.hpp"

#include "generation/provider.hpp"
#include "generation/file_data.hpp"
#include "generation/interface/part_descriptor.hpp"
#include "errors.h"

using namespace std::literals;
namespace mg = modegen::generation;
namespace mo = modegen::generation::options;
namespace mi = modegen::generation::interface;
namespace mp = modegen::parser;
namespace pi = modegen::parser::interface;
namespace pd = modegen::parser::data_tree;

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


MOCK_BASE_CLASS( mock_data_tree_loader, pd::loader )
{
	void load(std::istream& input, std::string fn) override {(void)input;(void)fn;}
	void load(FS::path file) override {(void)file;}
	//MOCK_METHOD( load, 2 )
	//MOCK_METHOD( load, 1 )
	MOCK_METHOD( finish_loads, 0 )
	MOCK_METHOD( boost_ptree, 0 )
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
BOOST_AUTO_TEST_CASE(few_mods)
{
	auto ldr = std::make_shared<fake_mi_loader>();
	ldr->data = pi::parse("module mod v1.0: int foo(); module other v1.0: int bar();"sv).mods;

	mo::container_ptr opt_cnt = std::make_shared<mo::container>();
	opt_cnt->raw().put("gen.def.output", "%mod%.ext");
	mi::part_descriptor pd(mo::view(opt_cnt, "def"sv), {ldr});

	BOOST_CHECK_EQUAL(pd.part_name(), "def");
	BOOST_CHECK_EQUAL(pd.need_output(), true);
	BOOST_CHECK_EQUAL(pd.file_name(), "mod.ext");
	BOOST_REQUIRE_EQUAL(pd.idl_input().size(), 1);
	BOOST_CHECK_EQUAL(pd.idl_input()[0].name, "mod");

	BOOST_CHECK_EQUAL(pd.next(), true);
	BOOST_CHECK_EQUAL(pd.part_name(), "def");
	BOOST_CHECK_EQUAL(pd.need_output(), true);
	BOOST_CHECK_EQUAL(pd.file_name(), "other.ext");
	BOOST_REQUIRE_EQUAL(pd.idl_input().size(), 1);
	BOOST_CHECK_EQUAL(pd.idl_input()[0].name, "other");

	BOOST_CHECK_EQUAL(pd.next(), false);
}
BOOST_AUTO_TEST_CASE(splited_by_ver_modules)
{
	auto ldr = std::make_shared<fake_mi_loader>();
	ldr->data = pi::parse("module mod v1.0: int foo(); @v2.0 int bar(); module other v1.0: int bar();"sv).mods;

	mo::container_ptr opt_cnt = std::make_shared<mo::container>();
	opt_cnt->raw().put("gen.def.output", "%mod%.ext");
	opt_cnt->raw().put("gen.def.versioning", "split");
	mi::part_descriptor pd(mo::view(opt_cnt, "def"sv), {ldr});

	BOOST_CHECK_EQUAL(pd.part_name(), "def");
	BOOST_CHECK_EQUAL(pd.need_output(), true);
	BOOST_CHECK_EQUAL(pd.file_name(), "mod1_0.ext");
	BOOST_REQUIRE_EQUAL(pd.idl_input().size(), 1);
	BOOST_CHECK_EQUAL(pd.idl_input()[0].name, "mod1_0");

	BOOST_CHECK_EQUAL(pd.next(), true);
	BOOST_CHECK_EQUAL(pd.part_name(), "def");
	BOOST_CHECK_EQUAL(pd.need_output(), true);
	BOOST_CHECK_EQUAL(pd.file_name(), "mod2_0.ext");
	BOOST_REQUIRE_EQUAL(pd.idl_input().size(), 1);
	BOOST_CHECK_EQUAL(pd.idl_input()[0].name, "mod2_0");

	BOOST_CHECK_EQUAL(pd.next(), true);
	BOOST_CHECK_EQUAL(pd.part_name(), "def");
	BOOST_CHECK_EQUAL(pd.need_output(), true);
	BOOST_CHECK_EQUAL(pd.file_name(), "other1_0.ext");
	BOOST_REQUIRE_EQUAL(pd.idl_input().size(), 1);
	BOOST_CHECK_EQUAL(pd.idl_input()[0].name, "other");

	BOOST_CHECK_EQUAL(pd.next(), false);
}
BOOST_AUTO_TEST_SUITE(wrong)
BOOST_AUTO_TEST_CASE(wrong_loader)
{
	// test may be only idl or data for now
	// tests: cannot dynamic cast
	auto ldr = std::make_shared<fake_mi_loader>();
	ldr->data = pi::parse("module mod v1.0: int foo(); @v2.0 int bar(); module other v1.0: int bar();"sv).mods;

	mo::container_ptr opt_cnt = std::make_shared<mo::container>();
	opt_cnt->raw().put("gen.def.output", "%mod%.ext");
	BOOST_CHECK_NO_THROW(mi::part_descriptor(mo::view(opt_cnt, "def"sv), {nullptr, ldr}));

	struct wrong_ldr : public mp::loader {
		void load(std::istream& input, std::string fn) override {}
		void load(FS::path file) override {}
		void finish_loads() override {}
	};
	BOOST_CHECK_NO_THROW(mi::part_descriptor(mo::view(opt_cnt,"def"sv), {nullptr, ldr, std::make_shared<wrong_ldr>()}));
	mi::part_descriptor pd(mo::view(opt_cnt, "def"sv), {nullptr, ldr, std::make_shared<wrong_ldr>()});
	BOOST_CHECK_EQUAL( pd.idl_input().size(), 1 );
	BOOST_CHECK( pd.next() );
	BOOST_CHECK_EQUAL( pd.idl_input().size(), 1 );
	BOOST_CHECK( !pd.next() );
}
BOOST_AUTO_TEST_CASE(extra_data_loader)
{
	auto ldr = std::make_shared<mock_data_tree_loader>();
	boost::property_tree::ptree ex_data;
	ex_data.put("some", "data");
	MOCK_EXPECT( ldr->boost_ptree ).once().returns( ex_data );

	mo::container_ptr opts_cnt = std::make_shared<mo::container>();
	mi::part_descriptor pd(mo::view(opts_cnt, "def"sv), {ldr});
	BOOST_CHECK_EQUAL( pd.data_input().get<std::string>("some"), "data"s );
}
BOOST_AUTO_TEST_SUITE_END() // wrong
BOOST_AUTO_TEST_SUITE_END() // interface
