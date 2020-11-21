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

namespace gen_utils {

enum class compiler { cpp } ;

class compilation_config {
public:
	virtual ~compilation_config() noexcept =default ;

	[[nodiscard]]
	virtual compiler compiler_name () const =0 ;

	[[nodiscard]]
	virtual std::string_view value(std::string_view key) const =0 ;
};

class tree;
class data_node;
using node_ptr = std::shared_ptr<data_node>;

struct variable {
	std::pmr::string name;
	std::pmr::string value;
};

class dsl_manager {
public:
	virtual ~dsl_manager() noexcept =default ;

	[[nodiscard]] virtual std::string_view id() const =0 ;
	[[nodiscard]] virtual boost::json::value to_json(
			const compilation_config& cfg, const tree& container) const =0 ;
};

class data_node {
public:
	virtual ~data_node() noexcept =default ;
	[[nodiscard]] virtual std::string_view name() const =0 ;
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

	bool node_exists(const data_node* n) const ;
	node_ptr create_link(const data_node* p, node_ptr c);
public:
	typedef std::function<bool(const data_node&)> copy_condition;
	tree() =delete;
	tree(node_ptr root, std::shared_ptr<dsl_manager> dm);

	[[nodiscard]] boost::json::value to_json(const compilation_config& cfg) const ;
	[[nodiscard]] std::string_view data_id() const ;
	[[nodiscard]] const data_node& root() const ;

	void add(const data_node& par, node_ptr child);
	[[nodiscard]] std::pmr::vector<node_ptr> children(const data_node& par) const ;

	[[nodiscard]] std::uint64_t root_version() const ;
	void root_version(std::uint64_t v) ;
	[[nodiscard]] std::uint64_t next_min_version() const ;

	[[nodiscard]] std::pmr::vector<std::pmr::string> var_name_list() const ;
	[[nodiscard]] std::pmr::vector<std::pmr::string> var_value_list(std::string_view name) const ;

	[[nodiscard]] tree copy(const copy_condition& cond) const ;
};

} // namesapce gen_utils
