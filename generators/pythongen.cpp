#include "pythongen.hpp"

#include <boost/process.hpp>


modegen::generation::jinja_file_generator::jinja_file_generator(std::filesystem::path gen)
	: generator(std::move(gen))
{
}

void modegen::generation::jinja_file_generator::operator () (std::filesystem::path tmpl, std::filesystem::path out, const cppjson::value& data) const
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
}

