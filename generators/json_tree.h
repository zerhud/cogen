#pragma once

#include "generator.hpp"

namespace modegen {
TODO(change namespace to generation)
namespace generators {

class json_tree : public modegen::generator
{
	boost::property_tree::ptree opts;
public:
	boost::property_tree::ptree& options() override {return opts;}
	const boost::property_tree::ptree& options() const override {return opts;}

	void path(const std::string& name, const std::string& value) override {}
	void create_definitions(generation_request query, std::vector<modegen::module> mods) const override;
};

} // namespace generators
} // namespace modegen
