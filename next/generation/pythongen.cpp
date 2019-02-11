#include "pythongen.hpp"

#include <exception>
#include <boost/process.hpp>


modegen::generation::jinja_python_generator::jinja_python_generator(FS::path gen)
	: generator(std::move(gen))
{
	if(generator.empty()) throw std::runtime_error("jinja pythone generator cannot work without path to generator");
	if(!FS::exists(generator)) throw std::runtime_error("file " + generator.string() + " doesn't exists");
}

const modegen::generation::json_jinja_generator& modegen::generation::jinja_python_generator::operator () (FS::path tmpl, FS::path out, const cppjson::value& data) const
{
	using namespace boost::process;

	opstream pdata;
	child a(
		  generator.u8string() // pythongen_path()
		, "-t", tmpl.u8string() // tmpl_path(t)
		, "-o", out.u8string() // out_path.generic_u8string()
		, std_out > stdout
		, std_in < pdata
		);
	pdata << data << std::endl;
	pdata.pipe().close();
	a.wait();

	return *this;
}

