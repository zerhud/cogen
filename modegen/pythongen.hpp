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
#include <nlohman/json.hpp>
#include <pybind11/embed.h>

namespace modegen {
namespace generation {

#pragma GCC visibility push(hidden)
class python_evaluator {
	//NOTE: this should to be the first member (segfault if any)
	pybind11::scoped_interpreter python_guard;
public:
	typedef std::variant<std::string, FS::path> python_def_t;

	python_evaluator(nlohmann::json data);

	const python_evaluator& sys_path(const FS::path& dir) ;
	const python_evaluator& tmpl(const FS::path& tfile, const std::optional<FS::path>& out) const ;
	const python_evaluator& script(const python_def_t& def) const ;

	python_evaluator& add_emb_fnc(const std::string& name, const python_def_t& def) ;
private:
	std::string str_gen_data() const ;

	pybind11::dict globals;
	nlohmann::json gen_data;

	static std::string template_script;
	static std::string set_jinja_script;
};
#pragma GCC visibility pop

} // namespace generators
} // namespace modegen
