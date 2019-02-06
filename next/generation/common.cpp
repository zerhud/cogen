#include "common.hpp"

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
}

