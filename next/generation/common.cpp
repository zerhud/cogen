#include "common.hpp"

#include <boost/property_tree/info_parser.hpp>
#include "provider.hpp"
#include "errors.h"

using namespace std::literals;
namespace mg = modegen::generation;

mg::generator::generator(mg::provider_ptr p, const std::filesystem::path& i)
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

void mg::generator::generate(const std::filesystem::path& output_dir) const
{
	assert( prov );
	for(auto& part:opts.get_child("gen")) {
		cppjson::value data = generate_data(part.first);
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

	auto target_name = opts.get<std::string>("gen."s+std::string(part)+".target"s);
	auto tl = prov->target_data(target_name);
	auto tg = prov->target_generator(target_name);

	assert(tl);
	assert(tg);
	options_view props(opts, part);
	return tg->jsoned_data(std::move(tl), std::move(props));
}

std::filesystem::path mg::generator::output_path(std::string_view part) const
{
	std::string p(part);
	std::string path = "gen." + p + ".output";
	return opts.get(path, p + ".hpp");
}

std::filesystem::path mg::generator::tmpl_path(std::string_view part) const
{
	std::string p(part);
	std::string path = "gen." + p + ".input";
	std::filesystem::path input_file = opts.get(path, p + ".jinja");
	return info_directory / input_file;
}

