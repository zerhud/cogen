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
	MOCK_METHOD(to_json, 2)
};

MOCK_BASE_CLASS(data_node, gen_utils::data_node)
{
	MOCK_METHOD(name, 0)
	MOCK_METHOD(version, 0)
	MOCK_METHOD(node_var, 0)
	MOCK_METHOD(required_links, 0)
};

std::shared_ptr<gen_utils_mocks::data_node> make_node(
		std::optional<std::uint64_t> v
		, std::optional<std::pmr::string> name=std::nullopt
		, std::optional<std::pmr::string> value=std::nullopt
		, std::optional<std::pmr::string> node_name=std::nullopt
		, std::pmr::vector<gen_utils::name_t> links={}
)
{
	assert( (!name && !value) || (name && value) );
	auto ret = std::make_shared<gen_utils_mocks::data_node>();
	MOCK_EXPECT(ret->version).returns(v);
	if(name) MOCK_EXPECT(ret->node_var)
				.returns(gen_utils::variable{*name, *value});
	else MOCK_EXPECT(ret->node_var).returns(std::nullopt);
	MOCK_EXPECT(ret->required_links).returns(links);
	if(node_name) MOCK_EXPECT(ret->name).returns(*node_name);
	return ret;
}

struct base_tree_fixture {
	std::shared_ptr<gen_utils_mocks::dsl_manager> dmanager=
			std::make_shared<gen_utils_mocks::dsl_manager>();
	std::shared_ptr<gen_utils_mocks::data_node> main_node;
	std::optional<gen_utils::tree> _tree;

	gen_utils::tree& tree() {
		if(!_tree) _tree.emplace(main_node, dmanager);
		return *_tree;
	}

	base_tree_fixture() {
		using namespace std::literals;
		MOCK_EXPECT(dmanager->id).returns("data_id"sv);
		main_node = make_node(100);
	}
};

} // namespace gen_utils_mocks
