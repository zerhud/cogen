/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE utils_checker

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parse.hpp"
#include "grammar/all.hpp"
#include "utils/checker.hpp"

namespace ast = ix3::ast;
namespace txt = ix3::text;
using namespace std::literals;
using ix3::utils::checker;

BOOST_AUTO_TEST_CASE(few_modules)
{
	ast::file_content result;
	std::string data = "module mod1 v1.1: module mod2 v1.2:"s;
	result = txt::parse(txt::file_content, data);

	checker ch;
	BOOST_CHECK_NO_THROW( ch(std::move(result)) );

	data = "module mod3 v1.3:"s;
	result = txt::parse(txt::file_content, data);
	BOOST_CHECK_NO_THROW( ch(std::move(result)) );

	auto extracted = ch.extract_result();
	BOOST_REQUIRE_EQUAL( extracted.size(), 3 );
	BOOST_TEST( extracted[0].name == "mod1"s );
	BOOST_TEST( extracted[1].name == "mod2"s );
	BOOST_TEST( extracted[2].name == "mod3"s );

	BOOST_TEST( extracted[0].version.major_v == 1 );
	BOOST_TEST( extracted[1].version.major_v == 1 );
	BOOST_TEST( extracted[2].version.major_v == 1 );

	BOOST_TEST( extracted[0].version.minor_v == 1 );
	BOOST_TEST( extracted[1].version.minor_v == 2 );
	BOOST_TEST( extracted[2].version.minor_v == 3 );

	extracted = ch.extract_result();
	BOOST_REQUIRE_EQUAL( extracted.size(), 0 );
}

BOOST_AUTO_TEST_CASE(version)
{
	checker ch;
	ast::file_content result;

	std::string data = "module mod v1.2: @v1.1 record r{+type a;}"s;
	result = txt::parse(txt::file_content, data);
	BOOST_CHECK_THROW( ch(result), checker::parent_version_is_greater );

	data = "module mod v1.1: @v1.1 record r{+type a;}"s;
	result = txt::parse(txt::file_content, data);
	BOOST_CHECK_NO_THROW( ch(result) );

	data = "module mod v1.2: @v1.1 type f();"s;
	result = txt::parse(txt::file_content, data);
	BOOST_CHECK_THROW( ch(result), checker::parent_version_is_greater );

	data = "module mod v1.2: "
	       "@v1.2 type f(); "
	       "@v1.2 interface i{ type f() const ; } "
	       "@v1.2 type f(); "
	       "@v1.2 record r{+type a;} "
	       "@v1.2 type f(); "s;
	result = txt::parse(txt::file_content, data);
	ch(result);
	//BOOST_CHECK_NO_THROW( ch(result) );
}
