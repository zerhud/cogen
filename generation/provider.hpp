/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include "config.hpp"
#include FILESYSTEM
#include <nlohman/json.hpp>

#include "common.hpp"
#include "file_data.hpp"
#include "parser/loader.hpp"

namespace modegen::generation {

class tmpl_gen_env {
public:
	typedef std::variant<std::string, FS::path> script_descriptor;

	tmpl_gen_env(nlohmann::json data, const FS::path& tmpl);

	nlohmann::json& data() ;
	const nlohmann::json& data() const ;

	FS::path& tmpl() ;
	const FS::path& tmpl() const ;

	std::optional<FS::path> out_dir() const ;
	tmpl_gen_env& out_dir(const std::optional<FS::path>& p);

	tmpl_gen_env& emb_fnc(const std::string& name, const script_descriptor& fnc);
	std::map<std::string, script_descriptor> emb_fnc_list() const ;

	tmpl_gen_env& exec_after(const script_descriptor& fnc);
	tmpl_gen_env& exec_before(const script_descriptor& fnc);
	script_descriptor exec_after() const ;
	script_descriptor exec_before() const ;
private:
	nlohmann::json gen_data_;
	FS::path tmpl_path_;
	std::optional<FS::path> out_dir_;
	std::map<std::string, script_descriptor> extra_scripts_;
};

class provider {
public:
	virtual ~provider() noexcept =default ;
	[[deprecated("use parsers insteed")]]
	virtual parser::loader_ptr parser(std::string_view name) const =0 ;
	virtual std::vector<parser::loader_ptr> parsers() const =0 ;
	virtual file_data_ptr generator(std::string_view name) const =0 ;
	virtual void json_jinja(const tmpl_gen_env& data) const =0 ;
	virtual FS::path resolve_file(const FS::path& p, const FS::path& assumed, std::string_view gen_name) const =0 ;
};

} // namespace modegen::generation {
