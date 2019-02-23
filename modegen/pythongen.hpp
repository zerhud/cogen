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
#include <pybind11/embed.h>

namespace modegen {
namespace generation {

#pragma GCC visibility push(hidden)
class python_evaluator {
	//NOTE: this should to be the first member (segfault if any)
	pybind11::scoped_interpreter python_guard;
public:
	typedef std::variant<std::string, FS::path> python_def_t;

	python_evaluator(cppjson::value data);

	const python_evaluator& sys_path(const FS::path& dir) ;
	const python_evaluator& tmpl(const FS::path& tfile) const ;
	const python_evaluator& script(const python_def_t& def) const ;

	python_evaluator& add_emb_fnc(const std::string& name, const python_def_t& def) ;
private:
	pybind11::dict globals;
	cppjson::value gen_data;
};
#pragma GCC visibility pop

class json_jinja_generator {
public:
	typedef std::variant<std::string, FS::path> python_def_t;

	virtual ~json_jinja_generator() noexcept =default ;

	virtual const json_jinja_generator& execute_script(const python_def_t& def) const =0 ;
	virtual json_jinja_generator& emb_fnc(const std::string& name, const python_def_t& def) =0 ;
	virtual json_jinja_generator& emb_mod(const std::string& name, const python_def_t& def) =0 ;

	virtual const json_jinja_generator& operator() (FS::path tmpl, std::optional<FS::path> out, const cppjson::value& data) const =0 ;
};


class jinja_python_generator : public json_jinja_generator {
public:
	jinja_python_generator(FS::path gen);
	const json_jinja_generator& execute_script(const python_def_t& def) const override ;
	json_jinja_generator& emb_fnc(const std::string& name, const python_def_t& def) override;
	json_jinja_generator& emb_mod(const std::string& name, const python_def_t& def) override;
	const json_jinja_generator& operator () (FS::path tmpl, std::optional<FS::path> out, const cppjson::value& data) const override ;
private:
	FS::path generator;
};

} // namespace generators
} // namespace modegen
