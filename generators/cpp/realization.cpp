#include "realization.h"


#include "parser/helpers.hpp"

#include "../to_json.h"
#include "../split_by_version.hpp"

#include "generators/config.hpp"

#include "type_converter.h"

using namespace std::literals;
using modegen::cvt::name_conversion;

namespace modegen::generators::cpp {
struct json_extra_info : converters::to_json_aspect {
	std::string link_name;
	helpers::type_converter* cvt;
	json_extra_info(helpers::type_converter* c, std::string l) : cvt(c), link_name(l) {}
	void final_object(cppjson::value& jval) override
	{
		std::vector<std::string> incs;
		if(cvt) incs = cvt->includes();
		if(!link_name.empty()) incs.emplace_back(link_name);
		for(std::size_t i=0;i<incs.size();++i) jval["incs"][i]["n"] = incs[i];
		if(!link_name.empty()) jval["incs"][incs.size()-1]["local"] = true;
	}
	
	void as_object(cppjson::value& jval, const modegen::module& obj) override
	{
		jval["namespace"] = obj.name + "_v"s + get_version(obj).value("_"sv) ;
	}
};
} // modegen::generators::cpp

modegen::generators::cpp::realization::realization()
{
}

void modegen::generators::cpp::realization::generate(modegen::mod_selection query, std::vector<modegen::module> mods) const
{
	std::filesystem::path base;
	if(query.output.has_value()) base = *query.output;

	cvt::naming{naming_option()}(mods);

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

	json_extra_info igen(&tconv, solve_option("def"));
	cppjson::value jsoned = modegen::converters::to_json(mods, igen);
	add_extra_info(jsoned);

	generate(jsoned, "realization.hpp.jinja");
}

void modegen::generators::cpp::realization::gen_cpp(modegen::mod_selection query, std::vector<modegen::module> mods) const
{
	filter_by_selection(query, mods);
	helpers::type_converter(mods).includes();
	if(mods.size()==0 && !solve_bool_option("gen_empty")) return;

	json_extra_info igen(nullptr, solve_option("hpp"));
	cppjson::value jsoned = modegen::converters::to_json(mods, igen);
	add_extra_info(jsoned);

	generate(jsoned, "realization.cpp.jinja");
}

void modegen::generators::cpp::realization::gen_def(modegen::mod_selection query, std::vector<modegen::module> mods) const
{
	filter_by_selection(query, mods);

	helpers::type_converter tconv(mods);
	if(mods.size()==0 && !solve_bool_option("gen_empty")) return ;

	json_extra_info igen(&tconv, "");
	cppjson::value jsoned = modegen::converters::to_json(mods, igen);
	add_extra_info(jsoned);

	generate(jsoned, "declarations.hpp.jinja");
}

void modegen::generators::cpp::realization::set_out(std::filesystem::path base, std::string_view file, std::string_view part) const
{
	if( base == "-" || base.empty() ) out_path = "-";
	else if(file.empty()) out_path = base;
	else out_path = base / file;

	TODO( remove const_cast )
	auto* self = const_cast<realization*>(this);
	self->try_to_set_option( part, file.empty() ? base.filename().generic_u8string() : std::string(file) );

	TODO( is par_dir.empty possible? )
	auto par_dir = out_path.parent_path();
	if(!par_dir.empty()) std::filesystem::create_directories(par_dir);
}

modegen::cvt::name_conversion modegen::generators::cpp::realization::naming_option() const
{
	return cvt::from_string(solve_option("naming"));
}

void modegen::generators::cpp::realization::try_to_set_option(std::string_view name, std::string val)
{
	std::string _n(name);
	opts.put(_n, opts.get(_n,val) );
}

std::string modegen::generators::cpp::realization::solve_option(std::string_view name) const
{

	std::string _n(name);
	if(name == "hpp"sv) return opts.get(_n, "mod.hpp"s);
	if(name == "def"sv) return opts.get(_n, "declarations.hpp"s);
	if(name == "naming"sv) return opts.get(_n, "underscore"s);

	return opts.get(_n, ""s);
}

bool modegen::generators::cpp::realization::solve_bool_option(std::string_view name) const
{
	std::string _n(name);
	std::string ret = opts.get(_n, "0"s);
	return ret == "true"s || ret == "True"s || ret == "1"s;
}

void modegen::generators::cpp::realization::generate(const cppjson::value& data, std::string_view t) const
{
}

void modegen::generators::cpp::realization::add_extra_info(cppjson::value& cdata) const
{
	set_constructors_prefix(cdata);
	set_extra_namespaces(cdata);
}

void modegen::generators::cpp::realization::set_constructors_prefix(cppjson::value& cdata) const
{
	name_conversion naming = naming_option();

	if(naming==name_conversion::title_case) {
		cdata["cnst_prefix"] = "Create";
	} else if(naming==name_conversion::underscore) {
		cdata["cnst_prefix"] = "create_";
		cdata["ptr_postfix"] = "_ptr";
	} else {
		cdata["cnst_prefix"] = "create";
		cdata["ptr_postfix"] = "Ptr";
	}
}

void modegen::generators::cpp::realization::set_extra_namespaces(cppjson::value& cdata) const
{
	std::size_t i=0;
	for(auto& en:opts) {
		if(en.first=="namespace") {
			cdata["namespaces"][i++] = en.second.get_value<std::string>();
		}
	}
}
