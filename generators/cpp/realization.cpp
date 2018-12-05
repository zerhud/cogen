#include "realization.h"

#include <boost/process.hpp>

#include "to_json.h"
#include "type_converter.h"

void modegen::generators::cpp::realization::generate(modegen::mod_selection query, std::vector<modegen::module> mods) const
{
	using namespace boost::process;

	query.sel = module_content_selector::interface;

	filter_by_selection(query, mods);
	auto incs = helpers::type_converter(query.sel, mods).includes();
	cppjson::value jsoned = modegen::converters::to_json(mods);

	for(std::size_t i=0;i<incs.size();++i) jsoned["incs"][i] = incs[i];

	opstream pdata;
	child a(
	      "../generators/pythongen"
	    , "-t", "../generators/cpp/realization.hpp.jinja"
	    , "-o", (query.output ? query.output->generic_u8string() : "-")
	    , std_out > stdout
	    , std_in < pdata
	    );
	pdata << jsoned << std::endl;
	pdata.pipe().close();
	a.wait();
}
