#include "declarations_gen.h"

#include <boost/process.hpp>

#include "to_json.h"

modegen::gen_options modegen::generators::cpp::declarations::options() const
{
	return gopts;
}

void modegen::generators::cpp::declarations::options(modegen::gen_options opts)
{
	gopts = opts;
}

void modegen::generators::cpp::declarations::output(std::filesystem::path o)
{
	//TODO: check if file is cpp header
	output_file = o;
}

void modegen::generators::cpp::declarations::generate(std::vector<modegen::module> mods) const
{
	using namespace boost::process;

	cast_options(gopts, mods);
	opstream pdata;
	cppjson::value jsoned = modegen::converters::to_json(mods);

	child a(
	      "../generators/pythongen"
	    , "-t", "../generators/cpp/declarations.hpp.jinja"
	    , "-o", (output_file.generic_u8string().empty() ? "-" : output_file.generic_u8string())
	    , std_out > stdout
	    , std_in < pdata
	    );
	pdata << jsoned << std::endl;
	pdata.pipe().close();
	a.wait();
}
