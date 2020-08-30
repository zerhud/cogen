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

class input {
public:
	virtual ~input() noexcept =default;
	virtual nlohmann::json to_json() const =0 ;
	virtual std::shared_ptr<input> clone() const =0 ;
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
