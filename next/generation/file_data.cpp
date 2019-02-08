#include "file_data.hpp"

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
	return opts.get_child("gen."s+std::string(part));
}

const boost::property_tree::ptree& mg::options_view::gen_data() const
{
	return opts.get_child("gen"s);
}

