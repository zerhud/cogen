#include "common.hpp"

#include "provider.hpp"
#include "errors.h"

using namespace std::literals;
namespace mg = modegen::generation;

mg::generator::generator(mg::provider_ptr p, const FS::path& i)
	: prov(std::move(p))
	, info_directory(i)
{
	if(!prov) throw errors::gen_error("common"s, "cannot create generator without provider"s);
}

boost::property_tree::ptree& mg::generator::options()
{
	return opts;
}

const boost::property_tree::ptree& mg::generator::options() const
{
	return opts;
}

void mg::generator::generate(const FS::path& output_dir) const
{
	assert( prov );
	for(auto& part:opts.get_child("gen")) {
		cppjson::value data = generate_data(part.first);
		if(data.is_undefined()) throw errors::gen_error("common", "no data for output");
		prov->json_jinja( data, tmpl_path(part.first), output_dir / output_path(part.first) );
	}
}

void mg::generator::generate_stdout(std::string_view part) const
{
	assert( prov );
	cppjson::value data = generate_data(part);
	prov->json_jinja( data, tmpl_path(part), std::nullopt );
}

cppjson::value mg::generator::generate_data(std::string_view part) const
{
	assert( prov );

	auto parser_name = opts.get_optional<std::string>("gen."s+std::string(part)+".parser"s);
	auto target_name = opts.get_optional<std::string>("gen."s+std::string(part)+".target"s);
	if(!parser_name) parser_name = opts.get<std::string>("defaults.parser"s);
	if(!target_name) target_name = opts.get<std::string>("defaults.target"s);
	auto tp = prov->parser(*parser_name);
	auto tg = prov->generator(*target_name);

	assert(tg);
	options_view props(opts, part);
	return tg->jsoned_data(std::move(tp), std::move(props));
}

FS::path mg::generator::output_path(std::string_view part) const
{
	std::string p(part);
	std::string path = "gen." + p + ".output";
	return opts.get(path, p + u8".hpp");
}

FS::path mg::generator::tmpl_path(std::string_view part) const
{
	assert( prov );

	std::string p(part);
	std::string path = "gen." + p + ".input";
	FS::path input_file = opts.get(path, p + u8".jinja");

	auto target = opts.get("gen."s+p+".target"s,opts.get("defaults.target"s,""s));
	return prov->resolve_file(input_file, info_directory, target);
}

