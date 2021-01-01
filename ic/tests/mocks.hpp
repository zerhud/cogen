/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <turtle/mock.hpp>
#include "ic/core.hpp"
#include "common_utils/tests/mocks.hpp"

namespace mic = mdg::ic;

namespace icmocks {

MOCK_BASE_CLASS( provider, mic::provider )
{
	MOCK_METHOD(generate, 3)
};

MOCK_BASE_CLASS( configuration, mic::configuration )
{
	MOCK_METHOD(parts, 0)

	MOCK_METHOD(all_dsl, 0)

	MOCK_METHOD(split_versions, 1)
	MOCK_METHOD(naming, 1)
	MOCK_METHOD(tmpl_file, 1)
	MOCK_METHOD(map_tmpl, 1)
	MOCK_METHOD(lang, 1)
};

struct ic_fixture {
	std::shared_ptr<gen_utils_mocks::data_node> t1_root, t2_root, t3_root;
	std::shared_ptr<gen_utils_mocks::dsl_manager> t1_dsl, t2_dsl, t3_dsl;
	std::optional<gen_utils::tree> _t1, _t2, _t3;

	gen_utils::tree& t1() {
		if(!_t1) _t1.emplace(t1_root, t1_dsl);
		return *_t1;
	}

	gen_utils::tree& t2() {
		if(!_t2) _t2.emplace(t2_root, t2_dsl);
		return *_t2;
	}

	gen_utils::tree& t3() {
		if(!_t3) _t3.emplace(t3_root, t3_dsl);
		return *_t3;
	}

	ic_fixture()
	    : t1_root(gen_utils_mocks::make_node(1, "t1r"))
	    , t2_root(gen_utils_mocks::make_node(2, "t2r"))
	    , t3_root(gen_utils_mocks::make_node(3, "t3r"))
	    , t1_dsl(std::make_shared<gen_utils_mocks::dsl_manager>())
	    , t2_dsl(std::make_shared<gen_utils_mocks::dsl_manager>())
	    , t3_dsl(std::make_shared<gen_utils_mocks::dsl_manager>())
	{
	}
};

} // namespace icmocks
