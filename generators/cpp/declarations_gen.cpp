#include "declarations_gen.h"

#include "to_json.h"

#include <boost/process.hpp>

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
	cppjson::value jsoned = modegen::converters::to_json(mods);

	opstream pdata;
	pdata << jsoned;
}
