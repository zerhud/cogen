/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE utils_selector

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parse.hpp"
#include "grammar/all.hpp"
#include "utils/split_by_version.hpp"

namespace ast = ix3::ast;
namespace txt = ix3::text;
namespace udt = boost::unit_test::data;
using namespace std::literals;
using ix3::utils::split_by_version;

BOOST_AUTO_TEST_SUITE(simple)
BOOST_AUTO_TEST_CASE( dry_run )
{
	auto data = txt::parse(txt::file_content, "module mod v1.0: type low(); @v1.1 type high();"sv);
	BOOST_TEST( data.modules.size() == 1 );
	data.modules | split_by_version(true);
	BOOST_REQUIRE_EQUAL( data.modules.size(), 1 );
}
BOOST_AUTO_TEST_CASE( by_mod )
{
	auto data = txt::parse(txt::file_content, "module mod v1.0: type low(); @v1.1 type high();"sv);
	BOOST_TEST( data.modules.size() == 1 );
	data.modules | split_by_version(false);
	BOOST_REQUIRE_EQUAL( data.modules.size(), 2 );
	BOOST_TEST( data.modules[0].name == "mod" );
	BOOST_TEST( data.modules[1].name == "mod" );
	BOOST_TEST( data.modules[0].version.minor_v == 0 );
	BOOST_TEST( data.modules[1].version.minor_v == 1 );
	BOOST_TEST( data.modules[0].content.size() == 1 );
	BOOST_TEST( data.modules[1].content.size() == 2 );
}
BOOST_AUTO_TEST_SUITE_END() // simple

BOOST_AUTO_TEST_SUITE(inner_fields)
BOOST_AUTO_TEST_CASE(record)
{
	auto data = txt::parse(txt::file_content, "module mod v1.0: record r { +type low; @v1.1 +type high;}"sv);
	BOOST_TEST( data.modules.size() == 1 );
	data.modules | split_by_version(false);
	BOOST_REQUIRE_EQUAL( data.modules.size(), 2 );
	BOOST_TEST( data.modules[0].name == "mod" );
	BOOST_TEST( data.modules[1].name == "mod" );
	BOOST_TEST( data.modules[0].version.minor_v == 0 );
	BOOST_TEST( data.modules[1].version.minor_v == 1 );
	BOOST_TEST_REQUIRE( data.modules[0].content.size() == 1 );
	BOOST_TEST_REQUIRE( data.modules[1].content.size() == 1 );
	ast::record* m1r = boost::get<ast::record>(&data.modules[0].content[0].var);
	ast::record* m2r = boost::get<ast::record>(&data.modules[1].content[0].var);
	BOOST_TEST_REQUIRE( m1r );
	BOOST_TEST_REQUIRE( m2r );
	BOOST_TEST( m1r->name == "r" );
	BOOST_TEST( m2r->name == "r" );
	BOOST_TEST( m1r->members.size() == 1 );
	BOOST_TEST( m2r->members.size() == 2 );
}
BOOST_AUTO_TEST_CASE(interface)
{
	auto data = txt::parse(txt::file_content, "module mod v1.0: interface r { @v1.1 type low() const; type high() const;}"sv);
	BOOST_TEST( data.modules.size() == 1 );
	data.modules | split_by_version(false);
	BOOST_REQUIRE_EQUAL( data.modules.size(), 2 );
	BOOST_TEST( data.modules[1].name == "mod" );
	BOOST_TEST( data.modules[0].version.minor_v == 0 );
	BOOST_TEST( data.modules[1].version.minor_v == 1 );
	BOOST_TEST_REQUIRE( data.modules[0].content.size() == 1 );
	BOOST_TEST_REQUIRE( data.modules[1].content.size() == 1 );
}
BOOST_AUTO_TEST_SUITE_END() // inner_fields

