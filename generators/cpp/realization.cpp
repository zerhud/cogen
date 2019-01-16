#include "realization.h"

#include <boost/process.hpp>

#include "parser/helpers.hpp"

#include "../to_json.h"
#include "../split_by_version.hpp"

#include "generators/config.hpp"

#include "type_converter.h"

void modegen::generators::cpp::realization::set_option(const std::string &key, const std::string &val)
{
	options[key]=val;
}

void modegen::generators::cpp::realization::generate(modegen::mod_selection query, std::vector<modegen::module> mods) const
{
	std::filesystem::path base;
	if(query.output.has_value()) base = *query.output;

	convert(mods, naming_option());

	set_out(base, "", query.what_generate);
	if(query.what_generate=="def") gen_def(query, std::move(mods));
	else if(query.what_generate=="hpp") gen_hpp(query, std::move(mods));
	else if(query.what_generate=="cpp") gen_cpp(query, std::move(mods));
	else if(query.what_generate=="all") {
		set_out(base, "declarations.hpp", "def"); gen_def(query, std::move(mods));
		set_out(base, "mod.hpp", "hpp"); gen_hpp(query, mods);
		set_out(base, "mod.cpp", "cpp"); gen_cpp(query, mods);
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
	filter_by_selection(query, mods);
	helpers::type_converter tconv(mods);
	//converters::split_by_version()(mods);
	if(mods.size()==0 && !solve_bool_option("gen_empty")) return;

	cppjson::value jsoned = modegen::converters::to_json(mods, module_content_selector::interface);

	auto incs = tconv.includes();
	incs.emplace_back(solve_option("def"));
	for(std::size_t i=0;i<incs.size();++i) jsoned["incs"][i]["n"] = incs[i];
	jsoned["incs"][incs.size()-1]["local"] = true;

	std::cout << "g: " << jsoned << std::endl;
	generate(jsoned, "realization.hpp.jinja");
}

void modegen::generators::cpp::realization::gen_cpp(modegen::mod_selection query, std::vector<modegen::module> mods) const
{
	filter_by_selection(query, mods);
	helpers::type_converter(mods).includes();
	if(mods.size()==0 && !solve_bool_option("gen_empty")) return;

	cppjson::value jsoned = modegen::converters::to_json(mods, module_content_selector::enumeration);
	jsoned["incs"][0]["n"] = solve_option("hpp");
	jsoned["incs"][0]["local"] = true;

	generate(jsoned, "realization.cpp.jinja");
}

void modegen::generators::cpp::realization::gen_def(modegen::mod_selection query, std::vector<modegen::module> mods) const
{
	filter_by_selection(query, mods);

	auto incs = helpers::type_converter(mods).includes();
	if(mods.size()==0 && !solve_bool_option("gen_empty")) return ;

	cppjson::value jsoned = modegen::converters::to_json(mods);
	for(std::size_t i=0;i<incs.size();++i) jsoned["incs"][i]["n"] = incs[i];
	set_constructors_prefix(jsoned);

	generate(jsoned, "declarations.hpp.jinja");
}

void modegen::generators::cpp::realization::set_out(std::filesystem::path base, std::string_view file, std::string_view part) const
{
	if( base == "-" || base.empty() ) out_path = "-";
	else if(file.empty()) out_path = base;
	else out_path = base / file;

	auto* self = const_cast<realization*>(this);
	auto ppos = self->options.find(part);
	if(ppos == self->options.end()) self->options.emplace(std::make_pair(std::string(part), file.empty() ? base.filename().generic_u8string() : std::string(file)));

	// TODO: is par_dir.empty possible?
	auto par_dir = out_path.parent_path();
	if(!par_dir.empty()) std::filesystem::create_directories(par_dir);
}

modegen::generators::name_conversion modegen::generators::cpp::realization::naming_option() const
{
	return from_string(solve_option("naming"));
}

std::string modegen::generators::cpp::realization::solve_option(std::string_view name) const
{
	using namespace std::literals;

	auto pos = options.find(name);
	if(pos != options.end()) return pos->second;

	if(name == "hpp"sv) return "mod.hpp"s;
	if(name == "def"sv) return "declarations.hpp"s;
	if(name == "naming"sv) return "underscore"s;

	return ""s;
}

bool modegen::generators::cpp::realization::solve_bool_option(std::string_view name) const
{
	auto pos = options.find(name);
	if(pos==options.end()) return false;
	return pos->second.empty() || pos->second == "true" || pos->second == "True" || pos->second == "1" ;
}

void modegen::generators::cpp::realization::generate(const cppjson::value& data, std::string_view t) const
{
	using namespace boost::process;

	opstream pdata;
	child a(
		  pythongen_path()
		, "-t", tmpl_path(t)
		, "-o", out_path.generic_u8string()
		, std_out > stdout
		, std_in < pdata
		);
	pdata << data << std::endl;
	pdata.pipe().close();
	a.wait();
}

void modegen::generators::cpp::realization::set_constructors_prefix(cppjson::value& output_data) const
{
	name_conversion naming = naming_option();

	if(naming==name_conversion::title_case) {
		output_data["cnst_prefix"] = "Create";
	} else if(naming==name_conversion::underscore) {
		output_data["cnst_prefix"] = "create_";
		output_data["ptr_postfix"] = "_ptr";
	} else {
		output_data["cnst_prefix"] = "create";
		output_data["ptr_postfix"] = "Ptr";
	}
}
