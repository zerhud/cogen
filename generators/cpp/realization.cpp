#include "realization.h"

#include <boost/process.hpp>

#include "to_json.h"
#include "type_converter.h"

using namespace boost::process;

void modegen::generators::cpp::realization::set_option(const std::string &key, const std::string &val)
{
	options[key]=val;
}

void modegen::generators::cpp::realization::generate(modegen::mod_selection query, std::vector<modegen::module> mods) const
{
	if(query.output.has_value()) out_path = *query.output;
	else out_path = "-";


	if(query.what_generate=="def") gen_def(query, std::move(mods));
	else if(query.what_generate=="hpp") gen_hpp(query, std::move(mods));
	else if(query.what_generate=="cpp") gen_cpp(query, std::move(mods));
	else if(query.what_generate=="all") {
		out_path /= "mod.hpp"; gen_hpp(query, mods);
		out_path /= "mod.cpp"; gen_cpp(query, mods);
		out_path /= "declarations.hpp"; gen_def(query, std::move(mods));
	}
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
		, "-o", out_path.generic_u8string()
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
		, "-o", out_path.generic_u8string()
	    , std_out > stdout
	    , std_in < pdata
	    );
	pdata << jsoned << std::endl;
	pdata.pipe().close();
	a.wait();
}

void modegen::generators::cpp::realization::gen_def(modegen::mod_selection query, std::vector<modegen::module> mods) const
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
		, "-o", out_path.generic_u8string()
		, std_out > stdout
		, std_in < pdata
		);
	pdata << jsoned << std::endl;
	pdata.pipe().close();
	a.wait();
}
