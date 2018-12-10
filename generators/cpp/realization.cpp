#include "realization.h"

#include <boost/process.hpp>

#include "helpers.hpp"
#include "to_json.h"
#include "type_converter.h"

#include "generators/config.hpp"

using namespace boost::process;

void modegen::generators::cpp::realization::set_option(const std::string &key, const std::string &val)
{
	options[key]=val;
}

void modegen::generators::cpp::realization::generate(modegen::mod_selection query, std::vector<modegen::module> mods) const
{
	std::filesystem::path base;
	if(query.output.has_value()) base = *query.output;

	set_out(base, "", query.what_generate);
	if(query.what_generate=="def") gen_def(query, std::move(mods));
	else if(query.what_generate=="hpp") gen_hpp(query, std::move(mods));
	else if(query.what_generate=="cpp") gen_cpp(query, std::move(mods));
	else if(query.what_generate=="all") {
		set_out(base, "mod.hpp", "hpp"); gen_hpp(query, mods);
		set_out(base, "mod.cpp", "cpp"); gen_cpp(query, mods);
		set_out(base, "declarations.hpp", "def"); gen_def(query, std::move(mods));
	}
}

std::string modegen::generators::cpp::realization::pythongen_path()
{
	return get_self_dir() / "pythongen";
}

std::string modegen::generators::cpp::realization::tmpl_path(std::string_view tn)
{
	static std::filesystem::path tmpl_dir(templates_dir);
	return tmpl_dir / tn;
}

void modegen::generators::cpp::realization::gen_hpp(modegen::mod_selection query, std::vector<modegen::module> mods) const
{
	query.sel = module_content_selector::interface;

	filter_by_selection(query, mods);
	auto incs = helpers::type_converter(query.sel, mods).includes();
	if(mods.size()==0) return;

	cppjson::value jsoned = modegen::converters::to_json(mods);
	for(std::size_t i=0;i<incs.size();++i) jsoned["incs"][i]["n"] = incs[i];
	jsoned["incs"][incs.size()]["n"] = solve_option("def");
	jsoned["incs"][incs.size()]["local"] = true;

	opstream pdata;
	child a(
	      pythongen_path()
	    , "-t", tmpl_path("realization.hpp.jinja")
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
	// only realization for enum's functions...
	query.sel = module_content_selector::enumeration;

	filter_by_selection(query, mods);
	helpers::type_converter(query.sel, mods);
	if(mods.size()==0) return;

	cppjson::value jsoned = modegen::converters::to_json(mods);
	jsoned["incs"][0]["n"] = solve_option("hpp");
	jsoned["incs"][0]["local"] = true;

	//TODO: add header file in includes

	opstream pdata;
	child a(
	      pythongen_path()
	    , "-t", tmpl_path("realization.cpp.jinja")
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
	auto incs = helpers::type_converter(query.sel, mods).includes();
	if(mods.size()==0) return ;

	cppjson::value jsoned = modegen::converters::to_json(mods);
	for(std::size_t i=0;i<incs.size();++i) jsoned["incs"][i]["n"] = incs[i];

	child a(
	      pythongen_path()
	    , "-t", tmpl_path("declarations.hpp.jinja")
	    , "-o", out_path.generic_u8string()
	    , std_out > stdout
	    , std_in < pdata
	    );
	pdata << jsoned << std::endl;
	pdata.pipe().close();
	a.wait();
}

void modegen::generators::cpp::realization::set_out(std::filesystem::path base, std::string_view file, std::string_view part) const
{
	if( base == "-" || base.empty() ) out_path = "-";
	else if(file.empty()) out_path = base;
	else out_path = base / file;

	auto* self = const_cast<realization*>(this);
	auto ppos = self->options.find(part);
	if(ppos == self->options.end()) self->options.emplace(std::make_pair(std::string(part), file.empty() ? base.filename().generic_u8string() : std::string(file)));

	std::filesystem::create_directories(out_path.parent_path());
}

std::string modegen::generators::cpp::realization::solve_option(std::string_view name) const
{
	using namespace std::literals;

	auto pos = options.find(name);
	if(pos != options.end()) return pos->second;

	if(name == "hpp"sv) return "mod.hpp"s;
	if(name == "def"sv) return "declarations.hpp"s;

	return ""s;
}
