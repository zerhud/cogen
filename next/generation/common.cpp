#include "common.hpp"

#include <iostream>
#include <boost/property_tree/info_parser.hpp>
#include "provider.hpp"
#include "errors.h"

using namespace std::literals;
namespace mg = modegen::generation;

mg::generator::generator(mg::provider_ptr p, const std::filesystem::path& i)
	: prov(std::move(p))
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
}

void mg::generator::generate_stdout(std::string_view part) const
{
	assert( prov );
	cppjson::value data;
	boost::property_tree::write_info(std::cout, opts);
	prov->json_jinja( data, tmpl_path(part), std::nullopt );
}

std::filesystem::path mg::generator::output_path(std::string_view part) const
{
	std::string p(part);
	std::string path = "gen." + p + ".output";
	std::cout << path << std::endl;
	return opts.get(path, p + ".hpp");
}

std::filesystem::path mg::generator::tmpl_path(std::string_view part) const
{
	std::string p(part);
	std::string path = "gen." + p + ".input";
	std::cout << path << std::endl;
	return opts.get(path, p + ".jinja");
}

