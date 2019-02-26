/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "pythongen.hpp"

#include <iomanip>
#include <sstream>
#include <exception>
#include <boost/process.hpp>

using namespace std::literals;
namespace mg=modegen::generation;
namespace py=pybind11;

modegen::generation::python_evaluator::python_evaluator(nlohmann::json data)
    : gen_data(std::move(data))
    , globals(py::globals())
{
	script(set_jinja_script);
}

const mg::python_evaluator& mg::python_evaluator::sys_path(const FS::path& dir)
{
	std::stringstream script;
	script << u8"import sys"s << std::endl
	       << u8"sys.path.append("s << std::quoted(dir.u8string()) << u8")"s << std::endl
	       ;
	py::exec(script.str(), globals);
	return *this;
}

const mg::python_evaluator& mg::python_evaluator::tmpl(const FS::path& tfile, const std::optional<FS::path>& out) const
{
	using namespace py::literals;
	globals["template_file"] = tfile.u8string();
	if(out) globals["output_file"] = out->u8string();
	else globals["output_file"] = "-"s;
	globals["template_str_data"] = str_gen_data();
	script(template_script);
	return *this;
}

const mg::python_evaluator& mg::python_evaluator::script(const mg::python_evaluator::python_def_t& def) const
{
	const auto* str = std::get_if<std::string>(&def);
	const auto* file = std::get_if<FS::path>(&def);
	if(str) py::exec(*str, py::globals(), globals);
	else if(file) {
		std::stringstream code;
		code << u8"exec(open(" << std::quoted(file->u8string()) << ").read())" ;
		py::exec(code.str(), py::globals(), globals);
	}
	else {
		assert(false);
	}

	return *this;
}

mg::python_evaluator& mg::python_evaluator::add_emb_fnc(const std::string& name, const mg::python_evaluator::python_def_t& def)
{
	script(def);
	std::stringstream code;
	code << "jinja_env.globals["s << std::quoted(name) << "]=locals()[" << std::quoted(name) << "]"s ;
	script(code.str());

	return *this;
}

std::string modegen::generation::python_evaluator::str_gen_data() const
{
	std::stringstream out;
	out << gen_data;
	return out.str();
}
