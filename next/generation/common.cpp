#include "common.hpp"

namespace mg = modegen::generation;

mg::generator::generator(mg::provider_ptr p, const std::filesystem::path& i)
	: prov(std::move(p))
{
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

