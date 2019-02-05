#pragma once

#include <memory>
#include <filesystem>
#include <string_view>
#include <boost/property_tree/ptree.hpp>

#include "parser/interface/modegen.hpp"

namespace modegen {
namespace generation {

class json_jinja_generator; /// generates file from template and json
typedef std::shared_ptr<json_jinja_generator> jsin_jinja_gen_ptr;

class provider; ///< provides data for generation
typedef std::shared_ptr<provider> provider_ptr;

/// generates files from info file and parameters
class generator {
public:
	/// @param p provider @param i path to info file
	generator(provider_ptr p, const std::filesystem::path& i);

	/// override options in info file
	boost::property_tree::ptree& options() ;
	/// read overriden options fron info file
	/// (added as opposite for mutable options)
	const boost::property_tree::ptree& options() const ;

	/// generate files to output dir
	void generate(const std::filesystem::path& output_dir) const ;
	/// generate only one part into std::cout
	void generate_stdout(std::string_view part) const ;
private:
	provider_ptr prov;
	boost::property_tree::ptree opts;
};

} // namespace generation
} //  namespace modegen
