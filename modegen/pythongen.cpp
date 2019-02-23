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

modegen::generation::jinja_python_generator::jinja_python_generator(FS::path gen)
	: generator(std::move(gen))
{
	if(generator.empty()) throw std::runtime_error("jinja pythone generator cannot work without path to generator");
	if(!FS::exists(generator)) throw std::runtime_error("file " + generator.string() + " doesn't exists");
}

const mg::json_jinja_generator& mg::jinja_python_generator::execute_script(const mg::json_jinja_generator::python_def_t& def) const
{
	return *this;
}

mg::json_jinja_generator& mg::jinja_python_generator::emb_fnc(const std::string& name, const mg::json_jinja_generator::python_def_t& def)
{
	return *this;
}

mg::json_jinja_generator& mg::jinja_python_generator::emb_mod(const std::string& name, const mg::json_jinja_generator::python_def_t& def)
{
	return *this;
}

const modegen::generation::json_jinja_generator& modegen::generation::jinja_python_generator::operator () (FS::path tmpl, std::optional<FS::path> out, const cppjson::value& data) const
{
	using namespace boost::process;

	std::string out_dir = u8"-"s;
	if(out) out_dir = out->u8string();

	TODO(use boost.logs here)
	std::cout << "generate " << (out ? out->u8string() : u8"-"s) << "\tfrom\t" << tmpl << std::endl;

	opstream pdata;
	child a(
		  generator.u8string()  // pythongen_path()
		, "-d", "-"
		, "-t", tmpl.u8string() // tmpl_path(t)
		, "-o", out_dir         // out_path.generic_u8string()
		, std_out > stdout
		, std_in < pdata
		);
	pdata << data << std::endl;
	pdata.pipe().close();
	a.wait();

	return *this;
}


modegen::generation::python_evaluator::python_evaluator(cppjson::value data)
    : gen_data(std::move(data))
    , globals(py::globals())
{
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

const mg::python_evaluator& mg::python_evaluator::tmpl(const FS::path& tfile) const
{
	return *this;
}

const mg::python_evaluator& mg::python_evaluator::script(const mg::python_evaluator::python_def_t& def) const
{
	const auto* str = std::get_if<std::string>(&def);
	const auto* file = std::get_if<FS::path>(&def);
	if(str) py::exec(*str, globals, py::dict());
	else if(file) {
		py::exec(u8"import "s + file->filename().u8string());
	}
	else {
		assert(false);
	}

	return *this;
}
