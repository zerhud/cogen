#pragma once

#include <filesystem>
#include "modegen.hpp"

namespace modegen {

class loader {
public:
	loader();
	loader(std::filesystem::path main_dir, std::vector<std::filesystem::path> includes={});

	void load(std::istream& input);
	void load(std::filesystem::path file);

	std::vector<modegen::module> result() ;
private:
	std::filesystem::path cur_dir;
	std::vector<modegen::module> accum;
	std::vector<std::filesystem::path> incs;
};

} // namespace modegen
