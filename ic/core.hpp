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
#include <memory_resource>

#include "common_utils/naming.hpp"
#include "input.hpp"

namespace modegen::ic {

struct map_result {
	std::pmr::string map;
	input data;
};

/// manages outputs (configure input for output can use it)
class generation_part {
public:

	virtual ~generation_part() noexcept =default;

	[[nodiscard]] virtual std::uint64_t id() const =0;
	[[nodiscard]] virtual std::string_view name() const =0;

	virtual void rename(gen_utils::name_conversion to) =0 ;
	virtual void split_versions() =0 ;

	virtual void map_to(std::string_view tmpl) =0;
	[[nodiscard]] virtual std::pmr::vector<map_result> compiled_input() const =0 ;
};

class output_generator {
public:
	virtual ~output_generator() noexcept =default ;
	virtual void add_search_path(std::filesystem::path dir) =0 ;
	virtual void generate(std::filesystem::path tmpl, const input& data) =0 ;
};

class factory {
public:
	virtual ~factory() noexcept =default ;

	[[nodiscard]] virtual std::shared_ptr<output_generator>
	create_generator(std::string_view name) const =0 ;

	[[nodiscard]] virtual std::shared_ptr<generation_part>
	create_part() const =0 ;
};

class configuration {
public:

	virtual ~configuration() noexcept =default;

	virtual void generate(std::filesystem::path tmpl_file, const map_result& data) const =0 ;

	[[nodiscard]] virtual input all_dsl() const =0 ;

	[[nodiscard]]
	virtual std::pmr::vector<std::shared_ptr<generation_part>>
	parts() const =0;

	[[nodiscard]] virtual bool split_versions(std::size_t id) const =0 ;
	[[nodiscard]] virtual gen_utils::name_conversion
	naming(std::size_t id) const =0 ;
	[[nodiscard]] virtual std::pmr::string tmpl_file(std::size_t id) const =0 ;
	[[nodiscard]] virtual std::pmr::string map_tmpl(std::size_t id) const =0 ;
};

class core {
	std::shared_ptr<factory> gen_system;
	void build(const configuration& config, generation_part& part) const ;
	void gen(const configuration& config, const generation_part& part) const ;
public:
	core(std::shared_ptr<factory> gs) ;
	void gen(const configuration& config) const ;
};

} // namespace modegen::ic
