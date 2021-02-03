/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <memory>
#include <string>
#include <optional>
#include <functional>
#include <memory_resource>
#include <boost/json.hpp>

#include "declarations.hpp"
#include "ic/declarations.hpp"
#include "naming.hpp"
#include "imports_manager.hpp"

namespace gen_utils {

struct variable {
	std::pmr::string name;
	std::pmr::string value;
};

class dsl_manager {
public:
	virtual ~dsl_manager() noexcept =default ;

	[[nodiscard]] virtual std::string_view id() const =0 ;
	[[nodiscard]] virtual boost::json::value to_json(
	        const compilation_context& cfg, const tree& container) const =0 ;
};

class data_node {
public:
	virtual ~data_node() noexcept =default ;
	[[nodiscard]] virtual std::string_view name() const =0 ;
	[[nodiscard]] virtual std::pmr::vector<name_t> required_links() const =0 ;
	[[nodiscard]] virtual std::string_view link_condition() const =0 ;
	[[nodiscard]] virtual std::optional<import_file> imports_modification() const =0 ;

	[[nodiscard]] virtual std::optional<std::uint64_t> version() const =0 ;
	[[nodiscard]] virtual std::optional<variable> node_var() const =0 ;
};

class tree final {
	struct edge {
		const data_node* parent;
		std::pmr::vector<node_ptr> children;
	};

	std::pmr::vector<node_ptr> store;
	std::uint64_t root_ver;
	std::shared_ptr<dsl_manager> dmanager;

	std::pmr::vector<edge> edges;

	edge* search_edge(const data_node& par) ;
	const edge* search_edge(const data_node& par) const ;

	node_ptr create_link(const data_node* p, node_ptr c);
	node_ptr create_link_if_no(const data_node* p, node_ptr c);
	std::pmr::vector<node_ptr> search(const data_node& par, name_t n) const ;
public:
	typedef std::function<bool(const data_node&)> copy_condition;
	tree() =delete;
	tree(node_ptr root, std::shared_ptr<dsl_manager> dm);

	[[nodiscard]] boost::json::value to_json(const compilation_context& ctx) const ;
	[[nodiscard]] std::string_view data_id() const ;
	[[nodiscard]] const data_node& root() const ;

	tree& add(const data_node& par, node_ptr child);
	[[nodiscard]] std::pmr::vector<node_ptr> children(const data_node& par) const ;
	[[nodiscard]] tree_compare_result contains(const tree& other) const ;
	[[nodiscard]] std::pmr::vector<node_ptr> search(name_t n) const ;
	[[nodiscard]] bool node_exists(const data_node* n) const ;

	[[nodiscard]] std::uint64_t root_version() const ;
	void root_version(std::uint64_t v) ;
	[[nodiscard]] std::uint64_t next_min_version() const ;
	const data_node* parent(const data_node& child, const copy_condition cond=copy_condition{}) const ;

	[[nodiscard]] std::pmr::vector<std::pmr::string> var_name_list() const ;
	[[nodiscard]] std::pmr::vector<std::pmr::string> var_value_list(std::string_view name) const ;

	void merge(tree other) ;
	[[nodiscard]] std::optional<tree> copy_if(const copy_condition& cond) const ;
};

} // namesapce gen_utils
