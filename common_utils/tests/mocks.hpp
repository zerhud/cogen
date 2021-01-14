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
	MOCK_METHOD(required_links, 0)
	MOCK_METHOD(link_condition, 0)
	MOCK_METHOD(imports_modification, 0)

	MOCK_METHOD(version, 0)
	MOCK_METHOD(node_var, 0)
};

template<typename T, typename Vec>
bool vec_contains(const Vec& vec, const T& obj)
{
	auto pos = std::find(vec.begin(), vec.end(), obj);
	return pos != vec.end();
}

template<typename VecLeft, typename VecRight>
void check_vec_eq(const VecLeft& l, const VecRight& r)
{
	BOOST_TEST( l.size() == r.size() );
	for(auto& i:l) BOOST_CHECK( vec_contains(r, i) );
}

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

struct trees_fixture {
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

	trees_fixture()
	    : t1_root(gen_utils_mocks::make_node(1, std::nullopt, std::nullopt, "t1r"))
	    , t2_root(gen_utils_mocks::make_node(2, std::nullopt, std::nullopt, "t2r"))
	    , t3_root(gen_utils_mocks::make_node(3, std::nullopt, std::nullopt, "t3r"))
	    , t1_dsl(std::make_shared<gen_utils_mocks::dsl_manager>())
	    , t2_dsl(std::make_shared<gen_utils_mocks::dsl_manager>())
	    , t3_dsl(std::make_shared<gen_utils_mocks::dsl_manager>())
	{
		MOCK_EXPECT(t1_dsl->id).returns("t1_id");
		MOCK_EXPECT(t2_dsl->id).returns("t2_id");
		MOCK_EXPECT(t3_dsl->id).returns("t3_id");
	}
};

} // namespace gen_utils_mocks
