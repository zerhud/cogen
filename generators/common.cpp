#include "common.h"


modegen::generation::common_generator::common_generrator(std::shared_ptr<modegen::generator_maker> gm) : gen_provider(std::move(gm)) {}

boost::property_tree::ptree& modegen::generation::common_generator::options() {return opts;}

const boost::property_tree::ptree& modegen::generation::common_generator::options() const {return opts;}

void modegen::generation::common_generator::create_definitions(modegen::generation_request query, std::vector<modegen::module> mods) const
{
	// parser info file (we cannot get it before?)
}
