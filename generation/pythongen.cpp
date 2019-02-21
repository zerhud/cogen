/*************************************************************************
 * Copyright © 2019 Hudyaev Alexy <hudyaev.alexy@gmail.com>
 * This file is part of modegen.
 * Distributed under the GNU Affero General Public License.
 * See accompanying file copying (at the root of this repository)
 * or <http://www.gnu.org/licenses/> for details
 *************************************************************************/

#include "pythongen.hpp"

#include <exception>
#include <boost/process.hpp>

using namespace std::literals;

modegen::generation::jinja_python_generator::jinja_python_generator(FS::path gen)
	: generator(std::move(gen))
{
	if(generator.empty()) throw std::runtime_error("jinja pythone generator cannot work without path to generator");
	if(!FS::exists(generator)) throw std::runtime_error("file " + generator.string() + " doesn't exists");
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

