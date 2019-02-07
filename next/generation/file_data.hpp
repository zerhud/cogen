#pragma once

#include <memory>
#include "parser/loader.hpp"

namespace modegen::generation {

class file_data ;
typedef std::shared_ptr<file_data> file_data_ptr;

/// options for generations (read only)
class options_view final {
	const boost::property_tree::ptree& opts;
	std::string_view part;
public:
	options_view(const boost::property_tree::ptree& o, std::string_view p);

	std::string_view part_name() const ;
	boost::property_tree::ptree part_data() const ;
	const boost::property_tree::ptree& gen_data() const ;
};

/// provides data for concreate file generation
class file_data {
public:
	virtual ~file_data() noexcept =default ;
	virtual cppjson::value jsoned_data(parser::loader_ptr data_loader, options_view opts) const =0 ;
};

} // namespace modegen::generation {
