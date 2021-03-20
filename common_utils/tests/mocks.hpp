/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of cogen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <turtle/mock.hpp>
#include "utils/tree.hpp"

std::pmr::string operator "" _s (const char* d, std::size_t l)
{
	return std::pmr::string(d, l);
}

boost::json::value operator "" _bj(const char* d, std::size_t l)
{
	boost::json::parse_options opts{.allow_trailing_commas=true};
	return boost::json::parse(
	            boost::json::string_view(d,l),
	            boost::json::storage_ptr(),
	            opts);
}

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
	for(auto& i:l) {
		BOOST_TEST_CONTEXT("checing " << i) {
			BOOST_CHECK( vec_contains(r, i) );
		}
	}
}

template<typename VecLeft, typename T>
void check_vec_eq(const VecLeft& l, std::initializer_list<T> r)
{
	BOOST_TEST( l.size() == r.size() );
	for(auto& i:r) {
		BOOST_TEST_CONTEXT("checking " << i) {
			BOOST_CHECK( vec_contains(l, i) );
		}
	}
}

struct node_info {
	std::optional<std::uint64_t> version;
	std::optional<std::pmr::string> name;
	std::optional<gen_utils::variable> node_var;
	std::pmr::vector<gen_utils::name_t> links;
	std::optional<std::pmr::string> link_cond;
	std::optional<gen_utils::import_file> import_mods;
};

struct node_st_info {
	std::optional<std::int64_t> parent;
	node_info child_data;
};

std::shared_ptr<gen_utils_mocks::data_node> mk_node(const node_info& data)
{
	auto ret = std::make_shared<gen_utils_mocks::data_node>();
	MOCK_EXPECT(ret->version).returns(data.version);
	if(data.name) MOCK_EXPECT(ret->name).returns(data.name.value());
	MOCK_EXPECT(ret->node_var).returns(data.node_var);
	MOCK_EXPECT(ret->required_links).returns(data.links);
	MOCK_EXPECT(ret->imports_modification).returns(data.import_mods);
	if(data.link_cond) MOCK_EXPECT(ret->link_condition).returns(*data.link_cond);
	else MOCK_EXPECT(ret->link_condition).returns("");
	return ret;
}

std::shared_ptr<data_node> mk_node(
        gen_utils::tree& con,
        const gen_utils::data_node& par,
        const node_info& data)
{
	auto ret = mk_node(data);
	con.add(par, ret);
	return ret;
}

std::vector<std::shared_ptr<data_node>> mk_tree(
        gen_utils::tree& con, std::vector<node_st_info> nodes)
{
	std::vector<std::shared_ptr<data_node>> created_nodes;
	for(auto& n:nodes) created_nodes.emplace_back(mk_node(n.child_data));
	for(std::size_t i=0;i<nodes.size();++i) {
		auto pi = nodes[i].parent;
		const gen_utils::data_node* par = nullptr;
		if(!pi.has_value()) par = &con.root();
		else  par = created_nodes.at(0 <= *pi ? *pi : i + *pi).get();
		con.add(*par, created_nodes[i]);
	}
	return created_nodes;
}

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
	    : t1_root(gen_utils_mocks::mk_node({.version=1, .name="t1r"}))
	    , t2_root(gen_utils_mocks::mk_node({.version=2, .name="t2r"}))
	    , t3_root(gen_utils_mocks::mk_node({.version=3, .name="t3r"}))
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
