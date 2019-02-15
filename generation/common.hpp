#pragma once

#include "config.hpp"

#include <memory>
#include FILESYSTEM
#include <string_view>
#include <cppjson/json.h>
#include <boost/property_tree/ptree.hpp>

#include "parser/interface/modegen.hpp"

namespace modegen {
namespace generation {

class provider; ///< provides data for generation
typedef std::shared_ptr<provider> provider_ptr;

class file_data ; ///< provides data for concreate file generation
typedef std::shared_ptr<file_data> file_data_ptr;

/// options for generations (read only)
class options_view;

/// generates files from info file and parameters
class generator {
public:
	/// @param p provider @param i path to info file
	generator(provider_ptr p, const FS::path& i);

	/// override options in info file
	boost::property_tree::ptree& options() ;
	/// read overriden options fron info file
	/// (added as opposite for mutable options)
	const boost::property_tree::ptree& options() const ;

	/// generate files to output dir
	void generate(const FS::path& output_dir) const ;
	/// generate only one part into std::cout
	void generate_stdout(std::string_view part) const ;
private:
	FS::path output_path(std::string_view part) const ;
	FS::path tmpl_path(std::string_view part) const ;
	cppjson::value generate_data(std::string_view part) const ;

	provider_ptr prov;
	boost::property_tree::ptree opts;
	FS::path info_directory;
};

} // namespace generation
} //  namespace modegen
