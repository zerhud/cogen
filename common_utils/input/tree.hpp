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

namespace gen_utils {

class data_node;
using node_ptr = std::shared_ptr<data_node>;

struct variable {
	std::string name;
	std::string value;
};

class data_node {
public:
	virtual ~data_node() noexcept =default ;
	[[nodiscard]] virtual std::string name() const =0 ;
	[[nodiscard]] virtual std::optional<std::uint64_t> version() const =0 ;
	[[nodiscard]] virtual std::optional<variable> node_var() const =0 ;
};

class tree final {
	struct edge {
		const data_node* parent;
		std::vector<node_ptr> children;
	};

	std::vector<node_ptr> store;
	std::uint64_t root_ver;
	std::string id;

	std::vector<edge> edges;

	bool node_exists(const data_node* n) const ;
	node_ptr create_link(const data_node* p, node_ptr c);
public:
	typedef std::function<bool(const data_node&)> copy_condition;
	tree() =delete;
	tree(node_ptr root, std::string id_);

	[[nodiscard]] std::string data_id() const ;
	[[nodiscard]] const data_node& root() const ;

	void add(const data_node& par, node_ptr child);
	[[nodiscard]] std::vector<node_ptr> children(const data_node& par) const ;

	[[nodiscard]] std::uint64_t root_version() const ;
	void root_version(std::uint64_t v) ;
	[[nodiscard]] std::uint64_t next_min_version() const ;

	[[nodiscard]] std::vector<std::string> var_name_list() const ;
	[[nodiscard]] std::vector<std::string> var_value_list(const std::string& name) const ;

	[[nodiscard]] tree copy(const copy_condition& cond) const ;
};

} // namesapce gen_utils
