#pragma once

#include <filesystem>
#include "check.h"
#include "modegen.hpp"

namespace modegen {

class loader {
public:
	loader();
	loader(std::vector<std::filesystem::path> includes);

	void load(std::istream& input, std::string fn);
	void load(std::filesystem::path file);

	std::vector<modegen::module> result() ;
private:
	bool already_loaded(const std::filesystem::path f) const ;
	std::filesystem::path search_file(std::filesystem::path f) const ;

	std::vector<std::filesystem::path> incs;
	std::vector<std::filesystem::path> cur_dir;
	std::vector<std::filesystem::path> loaded_files;

	checker ch;
};

} // namespace modegen
