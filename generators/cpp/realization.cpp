#include "realization.h"

#include <boost/process.hpp>

#include "to_json.h"
#include "type_converter.h"

using namespace boost::process;

void modegen::generators::cpp::realization::generate(modegen::mod_selection query, std::vector<modegen::module> mods) const
{
	gen_hpp(query, mods);
	std::cout << std::endl << std::endl;
	gen_cpp(query, std::move(mods));
}

void modegen::generators::cpp::realization::gen_hpp(modegen::mod_selection query, std::vector<modegen::module> mods) const
{
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

void modegen::generators::cpp::realization::gen_cpp(modegen::mod_selection query, std::vector<modegen::module> mods) const
{
	query.sel = module_content_selector::enumeration;

	filter_by_selection(query, mods);
	helpers::type_converter(query.sel, mods);
	cppjson::value jsoned = modegen::converters::to_json(mods);

	//TODO: add header file in includes

	opstream pdata;
	child a(
	      "../generators/pythongen"
	    , "-t", "../generators/cpp/realization.cpp.jinja"
	    , "-o", (query.output ? query.output->generic_u8string() : "-")
	    , std_out > stdout
	    , std_in < pdata
	    );
	pdata << jsoned << std::endl;
	pdata.pipe().close();
	a.wait();
}
