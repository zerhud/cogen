#pragma once

#include "../generator.hpp"

namespace modegen {
namespace generation {
namespace cpp {

class common : public generator {
public:
	common(std::shared_ptr<generation_provider> prov) : gen_prov(std::move(prov)) {}

	boost::property_tree::ptree& options() override {return opts;}
	const boost::property_tree::ptree& options() const override {return opts;}

	void path(const std::string& name, const std::string& value) override {}

	void create_definitions(generation_request query, std::vector<modegen::module> mods) const override ;
private:
	void generate(json_jinja_generator& gen, cppjson::value jsoned) const ;

	std::shared_ptr<generation_provider> gen_prov;
	boost::property_tree::ptree opts;
};

} // namespace cpp
} // namespace generation
} // namespace modegen
