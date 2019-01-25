#include "interface.h"
#include "converters.hpp"
#include "pythongen.hpp"

using namespace std::literals;

void modegen::generation::cpp::interface::create_definitions(modegen::generation_request query, std::vector<modegen::module> mods) const
{
	using namespace modegen::converters;
	type_converter tconv;
	json-extra_info igen(&tconv, path("def"sv));
	cppjson::value jsoned = mods | filter(query) | naming(query) | split_by_versions(query) | split_by_modules(query) | tconv | to_json(igen);

	jinja_file_generator gen(solve_option("generator"));
	gen(path("tdef"sv), path("def"sv), jsoned);
	gen(path("thpp"sv), path("hpp"sv), jsoned);
	gen(path("tcpp"sv), path("cpp"sv), jsoned);
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
