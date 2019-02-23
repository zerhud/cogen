/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#pragma once

#include <variant>
#include "config.hpp"
#include FILESYSTEM
#include <cppjson/json.h>

namespace modegen {
namespace generation {

class json_jinja_generator {
public:
	typedef std::variant<std::string, FS::path> python_def_t;

	virtual ~json_jinja_generator() noexcept =default ;

	virtual json_jinja_generator& emb_fnc(const std::string& name, const python_def_t& def) =0 ;
	virtual json_jinja_generator& emb_mod(const std::string& name, const python_def_t& def) =0 ;

	virtual const json_jinja_generator& operator() (FS::path tmpl, std::optional<FS::path> out, const cppjson::value& data) const =0 ;
};


class jinja_python_generator : public json_jinja_generator {
public:
	jinja_python_generator(FS::path gen);
	json_jinja_generator& emb_fnc(const std::string& name, const python_def_t& def) override {return *this;}
	json_jinja_generator& emb_mod(const std::string& name, const python_def_t& def) override {return *this;}
	const json_jinja_generator& operator () (FS::path tmpl, std::optional<FS::path> out, const cppjson::value& data) const override ;
private:
	FS::path generator;
};

} // namespace generators
} // namespace modegen
