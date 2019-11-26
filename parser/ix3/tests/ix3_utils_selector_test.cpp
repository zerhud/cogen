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
#include "utils/selector.hpp"

namespace ast = ix3::ast;
namespace udt = boost::unit_test::data;
using namespace std::literals;

BOOST_DATA_TEST_CASE(
	  selector
	, udt::make(
		 ast::module_content(ast::function{})
		,ast::enumeration{}
		,ast::record{}
		,ast::interface{}
		,ast::record{}
		)
	^ udt::make(
		 ix3::utils::selector::function
		,ix3::utils::selector::enumeration
		,ix3::utils::selector::record
		,ix3::utils::selector::interface
		,ix3::utils::selector::interface
		)
	^ udt::make(true, true, true, true, false)
	, content, selector, result)
{
	BOOST_TEST( ix3::utils::is_selected(content, selector) == result );
}

