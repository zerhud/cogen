/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE rules_common

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include "parse.hpp"
#include "grammar/common.hpp"
#include "operators/common.hpp"

using namespace std::literals;
namespace ast = ix3::ast;
namespace txt = ix3::text;

BOOST_AUTO_TEST_CASE(type)
{
	ast::type result;
	std::string data = "list<type>"s;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::type, data) );
	BOOST_TEST( result.name == "list"s );
	BOOST_TEST( result.sub_types.size() == 1 );
	BOOST_TEST( result.sub_types[0].get().name == "type"s );
	BOOST_TEST( result.sub_types[0].get().sub_types.size() == 0 );

	data = "list"s;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::type, data) );
	BOOST_TEST( result.name == "list"s );
	BOOST_TEST( result.sub_types.size() == 0 );

	data = "li st"s;
	BOOST_CHECK_THROW( result = txt::parse(txt::type, data), std::exception );

	data = "list<a,b>"s;
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::type, data) );
	BOOST_TEST( result.name == "list"s );
	BOOST_TEST( result.sub_types.size() == 2 );
}

BOOST_AUTO_TEST_CASE(docs)
{
	ast::meta::documentation result ;

	std::string data = "#kuku\n";
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::meta_documentation, data) );
	BOOST_TEST( result.body == "kuku"s );

	data = "# ku ku\n";
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::meta_documentation, data) );
	BOOST_TEST( result.body == "ku ku"s );
}

BOOST_AUTO_TEST_CASE(version)
{
	ast::meta::version result;

	std::string data = "v1.1";
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::meta_version, data) );
	BOOST_TEST( result.major_v == 1 );
	BOOST_TEST( result.minor_v == 1 );
}

BOOST_AUTO_TEST_CASE(depricated)
{
	ast::meta::depricated result;

	std::string data = "@depricated v1.1 (\"kuku kuku\")";
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::meta_depricated, data) );
	BOOST_TEST( result.message == "kuku kuku"s );
	BOOST_TEST( result.since.major_v == 1 );
	BOOST_TEST( result.since.minor_v == 1 );
}

BOOST_AUTO_TEST_CASE(meta_set)
{
	ast::meta::set result;

	std::string data = "@v1.1 @depricated v1.1 (\"kuku kuku\")#kuku\n";
	BOOST_CHECK_NO_THROW( result = txt::parse(txt::meta_set, data) );
	BOOST_TEST( result.cnt.size() == 3 );
	BOOST_TEST( boost::get<ast::meta::version>(result.cnt[0]).major_v == 1 );
}
