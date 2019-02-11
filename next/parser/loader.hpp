#pragma once

#include "config.hpp"
#include FILESYSTEM

namespace modegen::parser {

class loader;
typedef std::shared_ptr<loader> loader_ptr;

class loader {
public:
	virtual ~loader() noexcept =default ;

	virtual void load(std::istream& input, std::string fn) =0 ;
	virtual void load(FS::path file) =0 ;
	virtual void finish_loads() =0 ;
};

loader_ptr create_loader(std::string_view name, std::vector<FS::path> includes);

} // namespace modegen::parser


