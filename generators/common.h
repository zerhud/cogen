#include "generator.hpp"

namespace modegen {
namespace generation {

/// use other generators for generate files from info file
class common_generator : public generator {
public:
	common_generrator(std::shared_ptr<generator_maker> gm);

	void path(const std::string& name, const std::string& value) override {}

	boost::property_tree::ptree& options() override;
	const boost::property_tree::ptree& options() const override;


	void create_definitions(generation_request query, std::vector<modegen::module> mods) const override ;
private:
	std::shared_ptr<generator_maker> gen_provider;
	std::shared_ptr<generation_provider> info_provider;
	boost::property_tree::ptree opts;
};

} // namespace generation {
} // namespace modegen {
