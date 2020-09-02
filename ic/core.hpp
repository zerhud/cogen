/*************************************************************************
 * Copyright © 2020 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <memory>
#include <vector>
#include <filesystem>
#include <nlohman/json.hpp>
#include "common_utils/naming.hpp"

namespace modegen::ic {

class input_node {
public:
	virtual ~input_node() noexcept =default ;
	virtual void rename(std::function<std::string(const std::string&)> actor) =0 ;
	virtual std::uint64_t version() const =0;
	virtual std::uint64_t level() const =0 ; ///< level 0 is a root
	virtual std::shared_ptr<input_node> clone() const =0 ;
};

class input final {
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
public:
	input() =default ;
	~input() noexcept =default;

	std::vector<std::shared_ptr<input_node>> all() const ;
	std::vector<input_node*> children(const input_node* n) const ;

	void add(std::vector<std::shared_ptr<input_node>> list) ;
	void add(input_node* par, std::vector<std::shared_ptr<input_node>> list) ;
};

class output {
public:
	virtual ~output() noexcept =default;
	virtual void gen(std::string dir, std::string tmpl) const =0 ;
	virtual std::string name() const =0;
};

class generation_part {
public:
	virtual ~generation_part() noexcept =default;

	virtual std::size_t id() const =0;
	virtual std::string name() const =0;
	virtual std::vector<std::shared_ptr<output>> outputs() const =0;

	virtual void rename(gen_utils::name_conversion to) =0 ;
	virtual void split_versions() =0 ;
	virtual void map_to(std::string_view tmpl) =0;
};

class configuration {
public:

	virtual ~configuration() noexcept =default;

	virtual std::filesystem::path output_dir() const =0;

	virtual std::vector<std::shared_ptr<generation_part>> parts() const =0;

	virtual bool split_versions(std::size_t id) const =0 ;
	virtual gen_utils::name_conversion naming(std::size_t id) const =0 ;
	virtual std::string map_tmpl(std::size_t id) const =0 ;
	virtual std::string tmpl_information(std::size_t id) const =0;
};

class core {
public:
	core() ;
	void gen(
	          std::shared_ptr<output> out
	        , std::shared_ptr<configuration> config) const ;
};

} // namespace modegen::ic
