#include "declarations_gen.h"

#include <boost/process.hpp>

#include "to_json.h"
#include "type_converter.h"

void modegen::generators::cpp::declarations::generate(modegen::mod_selection query, std::vector<modegen::module> mods) const
{
	using namespace boost::process;

	filter_by_selection(query, mods);

	opstream pdata;
	auto incs = helpers::type_converter(module_content_selector::function, mods).includes();
	cppjson::value jsoned = modegen::converters::to_json(mods);

	for(std::size_t i=0;i<incs.size();++i) jsoned["incs"][i] = incs[i];

	child a(
	      "../generators/pythongen"
	    , "-t", "../generators/cpp/declarations.hpp.jinja"
	    , "-o", (query.output ? query.output->generic_u8string() : "-")
	    , std_out > stdout
	    , std_in < pdata
	    );
	pdata << jsoned << std::endl;
	pdata.pipe().close();
	a.wait();
}
