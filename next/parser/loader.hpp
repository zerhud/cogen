#pragma once

#include <filesystem>

namespace modegen::parser {

class loader;
typedef std::shared_ptr<loader> loader_ptr;

class loader {
public:
	virtual ~loader() noexcept =default ;

	virtual void load(std::istream& input, std::string fn) =0 ;
	virtual void load(std::filesystem::path file) =0 ;
};

loader_ptr create_loader(std::string_view name, std::vector<std::filesystem::path> includes);

} // namespace modegen::parser


