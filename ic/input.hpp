/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <functional>

namespace modegen::ic {

class input_node {
public:
	virtual ~input_node() noexcept =default ;
	virtual void rename(std::function<std::string(const std::string&)> actor) =0 ;
	[[nodiscard]] virtual std::optional<std::uint64_t> version() const =0;
	virtual std::uint64_t level() const =0 ; ///< level 0 is a root
	virtual std::shared_ptr<input_node> clone() const =0 ;
};

class [[deprecated("use gen_utils::tree insteed")]]
input final {
	struct edge {
		input_node* parent;
		std::vector<input_node*> children;
	};

	std::vector<edge> edges;
	std::vector<std::shared_ptr<input_node>> nodes;
	std::vector<input_node*> roots;

	std::vector<input_node*> to_pointers(
	        const std::vector<std::shared_ptr<input_node>>& list) const ;
	bool node_exists(input_node* node) const;
	void add(bool is_root, std::vector<std::shared_ptr<input_node>> list);
	void add_to_parent(input_node* par, std::vector<input_node*> list);
public:
	input() =default ;
	~input() noexcept =default;

	std::string check_tree_levels() const ;

	std::vector<std::shared_ptr<input_node>> all() const ;
	std::vector<input_node*> children(const input_node* n) const ;

	void add(std::vector<std::shared_ptr<input_node>> list) ;
	void add(input_node* par, std::vector<std::shared_ptr<input_node>> list) ;
};

} // namespace modegen::ic
