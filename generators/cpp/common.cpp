#include "common.h"

void modegen::generation::cpp::common::create_definitions(modegen::generation_request query, std::vector<modegen::module> mods) const
{
	using namespace modegen::cvt;
	assert( gen_prov );

	helpers::type_converter tconv;
	TODO(applay commented filters)
	cppjson::value jsoned = mods
	        | filter(query) | naming(query.naming)
	        | /*split_by_versions(query) | split_by_modules(query) |*/ tconv
	        | to_json(std::make_unique<json_extra_info>())
	        ;

	auto incs = tconv.includes();
	for(std::size_t i=0;i<incs.size();++i) jsoned["incs"][i]["n"] = incs[i];

	std::shared_ptr<json_jinja_generator> gen = provider->jinja_generator();
	generate(*gen, std::move(jsoned));
}

void modegen::generation::cpp::common::generate(json_jinja_generator& gen, cppjson::value jsoned) const
{
	// read info: files list, template and settings. for each file:
	// 	short name of file
	// 	include others files (ref to short name)
	// 	filter for file (not all will be include into file).
	// the file is interface.info

	//gen(path("tdef"sv), path("def"sv), add_includes({}, jsoned));
	//gen(path("thpp"sv), path("hpp"sv), add_includes({part_name("def"sv)}, jsoned));
	//gen(path("tcpp"sv), path("cpp"sv), add_includes({part_name("hpp"sv)}, jsoned));
}

