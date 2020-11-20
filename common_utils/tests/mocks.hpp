/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <turtle/mock.hpp>
#include "common_utils/input/tree.hpp"

namespace gen_utils_mocks {

MOCK_BASE_CLASS(dsl_manager, gen_utils::dsl_manager)
{
	MOCK_METHOD(id, 0)
	MOCK_METHOD(to_json, 1)
};

MOCK_BASE_CLASS(data_node, gen_utils::data_node)
{
	MOCK_METHOD(name, 0)
	MOCK_METHOD(version, 0)
	MOCK_METHOD(node_var, 0)
};

std::shared_ptr<gen_utils_mocks::data_node> make_node(
		std::optional<std::uint64_t> v
		, std::optional<std::pmr::string> name=std::nullopt
		, std::optional<std::pmr::string> value=std::nullopt
)
{
	assert( (!name && !value) || (name && value) );
	auto ret = std::make_shared<gen_utils_mocks::data_node>();
	MOCK_EXPECT(ret->version).returns(v);
	if(name) MOCK_EXPECT(ret->node_var)
				.returns(gen_utils::variable{*name, *value});
	else MOCK_EXPECT(ret->node_var).returns(std::nullopt);
	return ret;
}

} // namespace gen_utils_mocks
