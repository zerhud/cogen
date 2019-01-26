#include "interface.h"

#include "parser/helpers.hpp"

#include "pythongen.hpp"

#include "converters.hpp"
#include "converters/to_json.h"
#include "converters/case.h"
#include "converters/filter.h"
#include "type_converter.h"

using namespace std::literals;

namespace modegen::generation::cpp {
struct json_extra_info : cvt::to_json_aspect {
	void as_object(cppjson::value& jval, const modegen::module& obj) override
	{
		jval["namespace"] = obj.name + "_v"s + get_version(obj).value("_"sv) ;
	}
};
} // modegen::generators::cpp


void modegen::generation::cpp::interface::create_definitions(modegen::generation_request query, std::vector<modegen::module> mods) const
{
	using namespace modegen::cvt;
	helpers::type_converter tconv;
	TODO(applay commented filters)
	cppjson::value jsoned = mods
	        | filter(query) | naming(query.naming)
	        | /*split_by_versions(query) | split_by_modules(query) |*/ tconv
	        | to_json(std::make_unique<json_extra_info>())
	        ;

	auto incs = tconv.includes();
	for(std::size_t i=0;i<incs.size();++i) jsoned["incs"][i]["n"] = incs[i];

	jinja_file_generator gen(path("generator"sv));
	//gen(path("tdef"sv), path("def"sv), jsoned);
	//gen(path("thpp"sv), path("hpp"sv), jsoned);
	//gen(path("tcpp"sv), path("cpp"sv), jsoned);
}

std::filesystem::path modegen::generation::cpp::interface::path(std::string_view part) const 
{
	if(outputs.empty()) {
		if(part=="def"sv) return "declarations.hpp"s;
		if(part=="hpp"sv) return "module.hpp"s;
		if(part=="cpp"sv) return "module.hpp"s;
		if(part=="tdef"sv) return "declarations.hpp.jinja"s;
		if(part=="thpp"sv) return "module.hpp.jinja"s;
		if(part=="tcpp"sv) return "module.hpp.jinja"s;
		return ""s;
	}

	auto pos = outputs.find(part);
	if(pos==outputs.end()) return ""s;
	return pos->second;
}
