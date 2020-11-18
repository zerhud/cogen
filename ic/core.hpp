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

#include <boost/json.hpp>

#include "common_utils/naming.hpp"
#include "input.hpp"

namespace modegen::ic {

enum class json_generator { cpp, cmake, simple };

class provider {
public:
	virtual ~provider() noexcept =default;

	virtual void generate(
	        std::filesystem::path tmpl_file,
	        const boost::json::value& data,
	        std::string_view out_file) const =0 ;
};

class configuration {
public:

	virtual ~configuration() noexcept =default;

	[[nodiscard]] virtual input all_dsl() const =0 ;

	[[nodiscard]]
	virtual std::pmr::vector<std::string_view> parts() const =0;

	[[nodiscard]] virtual bool split_versions(std::string_view id) const =0 ;
	[[nodiscard]] virtual gen_utils::name_conversion
	naming(std::string_view id) const =0 ;
	[[nodiscard]] virtual std::pmr::string tmpl_file(std::string_view id) const =0 ;
	[[nodiscard]] virtual std::pmr::string map_tmpl(std::string_view id) const =0 ;
	[[nodiscard]] virtual json_generator lang(std::string_view id) const =0 ;
};

class core {
	std::shared_ptr<provider> prov;
public:
	core(std::shared_ptr<provider> p) ;
	void gen(const configuration& config) const ;
};

} // namespace modegen::ic
