#include "file_data.hpp"
#include "errors.h"

using namespace std::literals;
namespace mg = modegen::generation;

mg::options_view::options_view(const boost::property_tree::ptree& o, std::string_view p)
	: opts(o)
	, part(p)
{
}

std::string_view mg::options_view::part_name() const
{
	return part;
}

boost::property_tree::ptree mg::options_view::part_data() const
{
	return part_data(part);
}

const boost::property_tree::ptree& mg::options_view::gen_data() const
{
	return opts.get_child("gen"s);
}


boost::property_tree::ptree mg::options_view::part_data(std::string_view name) const
{
	auto part = opts.get_child_optional("gen."s+std::string(name));
	if(!part) throw errors::error("no options for part"s + std::string(name));
	return *part;
}
