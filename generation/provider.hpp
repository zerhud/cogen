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
#include <cppjson/json.h>

#include "common.hpp"
#include "file_data.hpp"
#include "parser/loader.hpp"

namespace modegen::generation {

class tmpl_gen_data {
public:
	tmpl_gen_data(cppjson::value data, const FS::path& tmpl);

	cppjson::value& data() ;
	const cppjson::value& data() const ;

	FS::path& tmpl() ;
	const FS::path& tmpl() const ;

	std::optional<FS::path> out_dir() const ;
	tmpl_gen_data& out_dir(const std::optional<FS::path>& p);

	boost::property_tree::ptree  generator_data() const ;
	tmpl_gen_data& generator_data(boost::property_tree::ptree d);
private:
	cppjson::value gen_data_;
	FS::path tmpl_path_;
	std::optional<FS::path> out_dir_;
	boost::property_tree::ptree data_for_generator_;
};

class provider {
public:
	virtual ~provider() noexcept =default ;
	virtual parser::loader_ptr parser(std::string_view name) const =0 ;
	virtual file_data_ptr generator(std::string_view name) const =0 ;
	virtual void json_jinja(const tmpl_gen_data& data) const =0 ;
	virtual FS::path resolve_file(const FS::path& p, const FS::path& assumed, std::string_view gen_name) const =0 ;
};

} // namespace modegen::generation {
